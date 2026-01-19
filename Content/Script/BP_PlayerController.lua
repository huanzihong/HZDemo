--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_ThirdPersonPlayerController_C
local M = UnLua.Class()

-- function M:Initialize(Initializer)
-- end

-- function M:UserConstructionScript()
-- end

function M:ReceiveBeginPlay()
    if self:IsLocalPlayerController() then
        local Widget = UE.UWidgetBlueprintLibrary.Create(self, UE.UClass.Load("/Game/UI/UI_Main.UI_Main_C"))
        Widget:AddToViewport()
    end
end

-- function M:ReceiveEndPlay()
-- end

-- function M:ReceiveTick(DeltaSeconds)
-- end

-- function M:ReceiveAnyDamage(Damage, DamageType, InstigatedBy, DamageCauser)
-- end

-- function M:ReceiveActorBeginOverlap(OtherActor)
-- end

-- function M:ReceiveActorEndOverlap(OtherActor)
-- end

return M
