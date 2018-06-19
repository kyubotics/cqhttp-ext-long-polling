#include <cqhttp/extension/extension.h>

#include "channel.h"

using namespace std;
using namespace cqhttp::extension;
using nlohmann::json;

static const int RETCODE_CONFLICT = 10100;

struct LongPolling : Extension {
    Info info() const override {
        static Info i;
        i.name = "长轮询";
        i.version = "0.0.2";
        i.build_number = 2;
        i.description = "提供类似于 Telegram Bot API 的长轮询接口。";
        return i;
    }

#define CONF_PREFIX "long_polling."

    void on_create(Context &ctx) override {
        enable_ = ctx.get_config_bool(CONF_PREFIX "enable", false);
        if (enable_) {
            auto max_queue_size = max({ctx.get_config_integer(CONF_PREFIX "max_queue_size", 2000), 0LL});
            chan_ = make_shared<Channel<json>>(max_queue_size);
            logger.debug(u8"事件数据缓冲队列创建成功");
        }
    }

    void on_event(EventContext &ctx) override {
        if (enable_) {
            chan_->put(ctx.data);
        }
    }

    void on_missed_action(ActionContext &ctx) override {
        if (!enable_ || ctx.action != "get_updates") {
            return;
        }

        const unique_lock<mutex> lock(mtx_, try_to_lock);
        if (!lock) {
            // there is another request getting updates now
            logger.debug(u8"有多个请求尝试调用 get_updates，已拒绝");
            ctx.result.code = RETCODE_CONFLICT;
            return;
        }

        const auto limit = ctx.get_param_integer("limit", 100);
        const auto timeout = ctx.get_param_integer("timeout", 0);

        if (limit <= 0 || timeout < 0) {
            ctx.result.code = ActionResult::Codes::DEFAULT_ERROR;
            return;
        }

        ctx.result.code = ActionResult::Codes::OK;
        ctx.result.data = json::array();

        json event;
        while (chan_->get(event, false)) {
            // get events ready in channel
            ctx.result.data.push_back(event);
            if (ctx.result.data.size() >= limit) {
                // it's enough
                break;
            }
        }
        if (timeout == 0 || !ctx.result.data.empty()) {
            // we've got events, or we couldn't wait
            return;
        }
        if (chan_->get(event, true, timeout * 1000)) {
            ctx.result.data.push_back(event);
            while (ctx.result.data.size() < limit && chan_->get(event, false)) {
                ctx.result.data.push_back(event);
            }
        }
    }

private:
    bool enable_{};
    shared_ptr<Channel<json>> chan_;
    mutex mtx_;
};

EXTENSION_CREATOR { return make_shared<LongPolling>(); }
