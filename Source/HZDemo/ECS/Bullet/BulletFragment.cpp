#include "BulletFragment.h"

#include "MassEntityTemplateRegistry.h"
#include "StructUtils/StructView.h"

void UBulletTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment(FConstStructView::Make(BulletFragment));
	BuildContext.AddTag<FBulletTag>();
}
