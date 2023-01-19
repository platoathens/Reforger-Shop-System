[BaseContainerProps()]
class ADM_MerchandiseVehicle: ADM_MerchandiseType
{
	EntitySpawnParams GetVehicleSpawnTransform(ADM_ShopComponent shop)
	{
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		shop.GetOwner().GetTransform(params.Transform);
		
		return params;
	}
	
	int lastCheckTime = -1;
	bool canRespawnCache = false;
	override bool CanRespawn(ADM_ShopComponent shop, int quantity = 1)
	{
		if (quantity > 1) quantity = 1;
		
		int curTick = System.GetTickCount();
		if (curTick - lastCheckTime >= 1000)
		{
			canRespawnCache = ADM_Utils.IsSpawnPositionClean(m_PrefabResource, GetVehicleSpawnTransform(shop), {shop.GetOwner()});
			lastCheckTime = curTick;
		}
		
		return canRespawnCache;
	}
	
	override bool CanDeliver(IEntity player, ADM_ShopComponent shop, int quantity = 1)
	{
		if (quantity > 1) quantity = 1;

		return CanRespawn(shop, quantity);
	}
	
	override bool Deliver(IEntity player, ADM_ShopComponent shop, int quantity = 1)
	{
		if (RplSession.Mode() == RplMode.Client) return false;
		if (quantity > 1) quantity = 1;
		
		// double check we can deliver
		bool canDeliver = this.CanDeliver(player, shop, quantity);
		if (!canDeliver) return false;
		
		// spawn vehicle
		EntitySpawnParams params = GetVehicleSpawnTransform(shop);
		IEntity entity = GetGame().SpawnEntityPrefab(m_PrefabResource, shop.GetOwner().GetWorld(), params);
		
		return true;
	}
}