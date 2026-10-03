#include "bridge/LeviBedrockReader.h"
#include "core/MinimapRenderer.h"
#include "core/TileCache.h"
#include "platform/OverlayWindow.h"

#include "ll/api/event/EventBus.h"
#include "ll/api/event/client/ClientExitLevelEvent.h"
#include "ll/api/event/world/ClientLevelTickEvent.h"
#include "ll/api/mod/NativeMod.h"
#include "ll/api/mod/RegisterHelper.h"

namespace novamap {

class NovaMapMod {
public:
    NovaMapMod() : mSelf(*ll::mod::NativeMod::current()) {}

    bool load() {
        mSelf.getLogger().info("Carregando Nova Map Client v0.2.0...");
        return true;
    }

    bool enable() {
        if (!mOverlay.create(mRenderer.size(), 1, 18)) {
            mSelf.getLogger().error("Falha ao criar o overlay do minimapa.");
            return false;
        }

        auto& bus = ll::event::EventBus::getInstance();
        mTickListener = bus.emplaceListener<ll::event::ClientLevelTickEvent>(
            [this](ll::event::ClientLevelTickEvent&) { tick(); },
            ll::event::EventPriority::Low
        );
        mExitListener = bus.emplaceListener<ll::event::ClientExitLevelEvent>(
            [this](ll::event::ClientExitLevelEvent&) {
                mCache.clear();
                mOverlay.setVisible(false);
            }
        );

        if (!mTickListener || !mExitListener) {
            disable();
            mSelf.getLogger().error("Falha ao registrar eventos do cliente.");
            return false;
        }

        mSelf.getLogger().info("Nova Map Client v0.2.0 ativo.");
        return true;
    }

    bool disable() {
        auto& bus = ll::event::EventBus::getInstance();
        if (mTickListener) { bus.removeListener(mTickListener); mTickListener.reset(); }
        if (mExitListener) { bus.removeListener(mExitListener); mExitListener.reset(); }
        mOverlay.destroy();
        mCache.clear();
        return true;
    }

    bool unload() { return true; }

private:
    void tick() noexcept {
        try {
            auto p = mWorld.getPlayerState();
            if (!p.valid) {
                mOverlay.setVisible(false);
                mOverlay.pump();
                return;
            }

            mCache.scanBudget(mWorld, p, 96);
            mRenderer.compose(mCache, p);
            mOverlay.present(mRenderer.pixels());
            mOverlay.pump();
        } catch (...) {
            mOverlay.setVisible(false);
        }
    }

    ll::mod::NativeMod& mSelf;
    LeviBedrockReader mWorld;
    TileCache mCache;
    MinimapRenderer mRenderer{128, 6};
    OverlayWindow mOverlay;
    ll::event::ListenerPtr mTickListener;
    ll::event::ListenerPtr mExitListener;
};

} // namespace novamap

LL_REGISTER_MOD(novamap::NovaMapMod, novamap::NovaMapMod);
