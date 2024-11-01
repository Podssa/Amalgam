#include "NoSpreadProjectile.h"

bool CNoSpreadProjectile::ShouldRun(CTFPlayer* pLocal, CTFWeaponBase* pWeapon)
{
	if (G::PrimaryWeaponType != EWeaponType::PROJECTILE)
		return false;

	switch (pWeapon->m_iItemDefinitionIndex())
	{
	case Soldier_m_RocketJumper:
	case Demoman_s_StickyJumper:
		return false;
	}

	return G::Attacking == 1;
}

void CNoSpreadProjectile::Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd)
{
	if (!ShouldRun(pLocal, pWeapon))
		return;

	SDK::RandomSeed(SDK::SeedFileLineHash(MD5_PseudoRandom(pCmd->command_number) & 0x7FFFFFFF, "SelectWeightedSequence", 0));
	for (int i = 0; i < 6; ++i)
		SDK::RandomFloat();

	switch (pWeapon->GetWeaponID())
	{
	case TF_WEAPON_SYRINGEGUN_MEDIC:
	{
		// don't let the _local_ syringes fool you (is there a way to fix or sync them?)
		pCmd->viewangles.x -= SDK::RandomFloat(-1.5f, 1.5f);
		pCmd->viewangles.y -= SDK::RandomFloat(-1.5f, 1.5f);

		G::PSilentAngles = true;
		return;
	}
	case TF_WEAPON_COMPOUND_BOW:
	{
		// ShouldRun huntsman
		if (pWeapon->As<CTFPipebombLauncher>()->m_flChargeBeginTime() > 0.f && I::GlobalVars->curtime - pWeapon->As<CTFPipebombLauncher>()->m_flChargeBeginTime() <= 5.0f)
			return;

		float flRand = float(SDK::RandomInt()) / 0x7FFF;
		pCmd->viewangles.x -= -6 + flRand * 12.f;
		flRand = float(SDK::RandomInt()) / 0x7FFF;
		pCmd->viewangles.y -= -6 + flRand * 12.f;

		G::PSilentAngles = true;
		return;
	}
	}

	if (pWeapon->m_iItemDefinitionIndex() == Soldier_m_TheBeggarsBazooka && !pWeapon->IsInReload())
	{
		float flOldCurrentTime = I::GlobalVars->curtime;
		I::GlobalVars->curtime = TICKS_TO_TIME(pLocal->m_nTickBase());
		pCmd->viewangles -= pWeapon->GetSpreadAngles() - I::EngineClient->GetViewAngles();
		I::GlobalVars->curtime = flOldCurrentTime;

		G::PSilentAngles = true;
	}
}