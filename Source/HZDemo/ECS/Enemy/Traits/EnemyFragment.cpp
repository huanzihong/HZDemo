#include "EnemyFragment.h"

#include "MassEntityTemplateRegistry.h"

void UEnemyTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment(FConstStructView::Make(EnemyFragment));
	BuildContext.AddTag<FEnemyTag>();
}
