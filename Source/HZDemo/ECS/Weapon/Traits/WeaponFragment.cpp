#include "WeaponFragment.h"

#include "MassEntityTemplateRegistry.h"
#include "StructUtils/StructView.h"

void UWeaponTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment(FConstStructView::Make(WeaponFragment));
	BuildContext.AddTag<FWeaponTag>();
}
