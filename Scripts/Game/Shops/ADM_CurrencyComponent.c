class SCR_CurrencyPredicate: InventorySearchPredicate
{
	void SCR_CurrencyPredicate()
	{
		QueryComponentTypes.Insert(ADM_CurrencyComponent);
	}

	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{		
		return (ADM_CurrencyComponent.Cast(queriedComponents[0])).GetValue() > 0;
	}
};

class ADM_CurrencyComponentClass: ScriptComponentClass
{
};

//! A brief explanation of what this component does.
//! The explanation can be spread across multiple lines.
//! This should help with quickly understanding the script's purpose.
class ADM_CurrencyComponent: ScriptComponent
{
	[Attribute(uiwidget: UIWidgets.EditBox, params: "1 inf")]
	protected int m_Value;
	
	int GetValue()
	{
		return m_Value;
	}
	
	static array<IEntity> FindCurrencyInInventory(SCR_InventoryStorageManagerComponent inventoryManager)
	{
		array<IEntity> currencyItems = {};
		SCR_CurrencyPredicate predicate = new SCR_CurrencyPredicate();
		inventoryManager.FindItems(currencyItems, predicate);
		
		return currencyItems;
	}
	
	static int FindTotalCurrencyInInventory(SCR_InventoryStorageManagerComponent inventoryManager)
	{
		int total = 0;
		
		array<IEntity> currency = ADM_CurrencyComponent.FindCurrencyInInventory(inventoryManager);
		foreach (IEntity currencyEntity: currency)
		{
			ADM_CurrencyComponent currencyComponent = ADM_CurrencyComponent.Cast(currencyEntity.FindComponent(ADM_CurrencyComponent));
			if (!currencyComponent) continue;
			
			total += currencyComponent.GetValue();
		}
		
		return total;
	}
	
	// Denomination is <bill amount, quantity>
	static array<ref Tuple2<int, int>> CountDenominations(notnull array<IEntity> currencyItems)
	{
		array<Tuple2<int, int>> denominations = {};
		foreach (IEntity currencyEnt : currencyItems)
		{
			ADM_CurrencyComponent currencyComp = ADM_CurrencyComponent.Cast(currencyEnt.FindComponent(ADM_CurrencyComponent));
			int denomination = currencyComp.GetValue();
			
			bool inArray = false;
			foreach (Tuple2<int, int> curDenomination : denominations)
			{
				if (curDenomination.param1 == denomination)
				{
					inArray = true;
					curDenomination.param2++;
					break;
				}
			}
			
			if (!inArray) denominations.Insert(new Tuple2<int, int>(denomination, 1));
		}
		
		array<ref Tuple2<int, int>> sortedDenominations = {}; // high to low
		foreach (Tuple2<int, int> denomination : denominations)
		{
			bool done = false;
			int denominationAmount = denomination.param1;
			
			for (int i = 0; i < sortedDenominations.Count(); i++)
			{
				int curDenominationAmount = sortedDenominations[i].param1;
				if (denominationAmount > curDenominationAmount)
				{
					sortedDenominations.InsertAt(denomination, i);
					done = true;
					break;
				}
			}
			
			if (!done)
				sortedDenominations.Insert(denomination);
		}
		
		return sortedDenominations;
	}
	
	static array<ref Tuple2<int, int>> FindGreedySolution(array<ref Tuple2<int, int>> givenDenominations, int amount)
	{
		array<ref Tuple2<int, int>> greedySolution = {};
		
		foreach (Tuple2<int, int> denomination : givenDenominations)
		{
			
		}
		
		return greedySolution;
	}
	
	static bool RemoveCurrencyFromInventory(SCR_InventoryStorageManagerComponent inventory, int amount)
	{
		if (!inventory) return false;
		if (amount < 1) return false;
		return false;
		
		/*
		array<IEntity> currencyItems = ADM_CurrencyComponent.FindCurrencyInInventory(inventory);
		array<ref Tuple2<int, int>> denominations = ADM_CurrencyComponent.CountDenominations(currencyItems);
		array<ref Tuple2<int, int>> requiredDenominations = ADM_CurrencyComponent.FindGreedySolution(denominations, amount);
		
		array<bool> removedItems = {};
		int totalMoneyRemoved = 0;
		foreach (Tuple2<int, int> denomination : requiredDenominations)
		{
			int denominationAmount = denomination.param1;
			int denominationQuantityToRemove = denomination.param2;
			
			foreach (IEntity currencyEntity : currencyItems)
			{
				ADM_CurrencyComponent currencyComponent = ADM_CurrencyComponent.Cast(currencyEntity.FindComponent(ADM_CurrencyComponent));
				if (currencyComponent.GetValue() == denominationAmount)
				{
					bool removedItem = inventory.TryDeleteItem(currencyEntity, null);
					removedItems.Insert(removedItem);
					totalMoneyRemoved += denominationAmount;
				}
					
				if (denominationQuantityToRemove <= 0)
					break;
			}
		}
		
		if (removedItems.Contains(false) || removedItems.Count() == 0)
			return false;
		
		int credit = totalMoneyRemoved - amount;
		if (credit < 0)
		{
			Print("Error! We didn't collect enough money!", LogLevel.ERROR);
			return false;
		}
		
		if (credit > 0)
		{
			Print(string.Format("Success! We owe the person $%1", credit));
		}
		
		return true;	*/
	}
}