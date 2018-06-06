#include <cqhttp/extension/extension.h>

#include "channel.h"

using namespace std;
using namespace cqhttp::extension;
using nlohmann::json;

struct LongPolling : Extension {
    Info info() const override {
        Info i;
        i.name = "长轮询";
        i.version = "0.0.1";
        i.build_number = 1;
        i.description = "提供类似于 Telegram Bot API 的长轮询接口。";
        return i;
    }

    void on_event(EventContext &ctx) override { chan_.put(ctx.data); }

    void on_missed_action(ActionContext &ctx) override {
        if (ctx.action != "get_updates") {
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
        while (chan_.get(event, false)) {
            // get events ready in channel
            ctx.result.data.push_back(event);
            if (ctx.result.data.size() >= limit) {
                // it's enough
                return;
            }
        }
        if (timeout == 0 || !ctx.result.data.empty()) {
            // we've got events, or we couldn't wait
            return;
        }
        chan_.get(event, true, timeout * 1000);
        ctx.result.data.push_back(event);
        while (ctx.result.data.size() < limit && chan_.get(event, false)) {
            ctx.result.data.push_back(event);
        }
    }

private:
    Channel<json> chan_;
};

PLUGIN_CREATOR { return make_shared<LongPolling>(); }
