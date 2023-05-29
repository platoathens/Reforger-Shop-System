class ADM_PhysicalShopAction : ScriptedUserAction
{
	//! Adjustment step of normalized value
	[Attribute( uiwidget: UIWidgets.Auto, defvalue: "1", desc: "Adjustment Step")]
	protected float m_fAdjustmentStep;
	
	//! Name of action to control the input
	[Attribute( uiwidget: UIWidgets.Auto, defvalue: "SelectAction", desc: "Input action for increase")]
	protected string m_sActionIncrease;
	
	//! Name of action to control the input
	[Attribute( uiwidget: UIWidgets.Auto, defvalue: "", desc: "Input action for decrease")]
	protected string m_sActionDecrease;
	
	protected IEntity m_Owner;
	protected ADM_PhysicalShopComponent m_Shop;
		
	protected string m_ItemName;
	
	protected float m_fTargetValue;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
	{
		m_Owner = pOwnerEntity;
		m_Shop = ADM_PhysicalShopComponent.Cast(m_Owner.FindComponent(ADM_PhysicalShopComponent));
		if (!m_Shop) 
		{
			Print("Error! Could not find physical shop component for this action.", LogLevel.ERROR);
			return;
		}
		
		array<ref ADM_ShopMerchandise> shopMerchandise = m_Shop.GetMerchandise();
		if (!shopMerchandise) return;
		
		ADM_ShopMerchandise merchandise;
		if (shopMerchandise.Count() > 0) merchandise = shopMerchandise[0];
		if (!merchandise) return;
		
		ADM_MerchandiseType merchandiseType = merchandise.GetMerchandise();
		if (!merchandiseType) return;
		
		m_ItemName = ADM_Utils.GetPrefabDisplayName(merchandiseType.GetPrefab());
		m_fTargetValue = m_fAdjustmentStep;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{	
		if (!m_Shop || m_Shop.GetMerchandise().Count() <= 0) return;
				
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController) return;
		
		SCR_PlayerController scrPlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!scrPlayerController) return;
		
		if (pUserEntity != scrPlayerController.GetMainEntity()) return;
		
		// Physical shops should only have one item defined, so just grab the first one in the merchandise array	
		ADM_ShopMerchandise merchandise = m_Shop.GetMerchandise()[0];
		if (ADM_ShopComponent.IsPaymentOnlyCurrency(merchandise) || !(merchandise.GetRequiredPaymentToBuy().Count() > 0))
		{
			ADM_PlayerShopManagerComponent playerShopManager = ADM_PlayerShopManagerComponent.Cast(playerController.FindComponent(ADM_PlayerShopManagerComponent));
			if (!playerShopManager) return;
			
			playerShopManager.AskPurchase(m_Shop, merchandise, m_fTargetValue);
		} else {
			m_Shop.ViewPayment(this);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Increment by adjustment step
	void HandleActionIncrease(float value)
	{
		if (m_fAdjustmentStep <= 0)
			return;
		
		if (value > 0.5)
			value = m_fAdjustmentStep;
		else if (value < -0.5)
			value = -m_fAdjustmentStep;
		else
			return;
		
		// Limit to normalized current value +/- adjustment limit
		float previousValue = m_fTargetValue;
		m_fTargetValue += value;
		
		// Round to adjustment step
		m_fTargetValue = Math.Round(m_fTargetValue / m_fAdjustmentStep) * m_fAdjustmentStep;
		if (m_fTargetValue <= m_fAdjustmentStep) m_fTargetValue = m_fAdjustmentStep; // minimum
		
		if (!float.AlmostEqual(m_fTargetValue, previousValue))
			SetSendActionDataFlag();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Decrement by adjustment step
	void HandleActionDecrease(float value)
	{
		HandleActionIncrease(-value);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!m_Shop || m_Shop.GetMerchandise().Count() <= 0) return false;
		
		VObject model = m_Shop.GetOwner().GetVObject();
		if (!model) return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_Shop || m_Shop.GetMerchandise().Count() <= 0) return false;
		
		//return m_Shop.CanPurchase(user);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (!m_Shop || m_Shop.GetMerchandise().Count() <= 0) return false;
		
		ADM_ShopMerchandise merchandise = m_Shop.GetMerchandise()[0];
		string actionName = "Purchase";
		if (!merchandise.GetRequiredPaymentToBuy().Count() > 0) actionName = "Free";
		if (m_ItemName.Length() > 0) actionName += string.Format(" %1", m_ItemName);
		
		bool currencyOnly = ADM_ShopComponent.IsPaymentOnlyCurrency(merchandise);
		if (currencyOnly) {
			int cost = ADM_PaymentMethodCurrency.Cast(merchandise.GetRequiredPaymentToBuy()[0]).GetQuantity();
			actionName += string.Format(" ($%1)", cost);
		}
		
		if (m_fAdjustmentStep > 0)
		{
			actionName += string.Format(" x%1", m_fTargetValue);
		}
		outName = actionName;
		
		return true;
	}
	
	//----------------------------------------------------------------------------------
	override bool CanBroadcastScript() 
	{ 
		return false; 
	};
	
	//----------------------------------------------------------------------------------
	override void OnActionSelected()
	{
		super.OnActionSelected();

		if (m_fAdjustmentStep <= 0)
			return;
		
		if (!m_sActionIncrease.IsEmpty())
			GetGame().GetInputManager().AddActionListener(m_sActionIncrease, EActionTrigger.VALUE, HandleActionIncrease);
		
		if (!m_sActionDecrease.IsEmpty())
			GetGame().GetInputManager().AddActionListener(m_sActionDecrease, EActionTrigger.VALUE, HandleActionDecrease);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActionDeselected()
	{
		super.OnActionDeselected();
		
		if (m_fAdjustmentStep <= 0)
			return;
		
		if (!m_sActionIncrease.IsEmpty())
			GetGame().GetInputManager().RemoveActionListener(m_sActionIncrease, EActionTrigger.VALUE, HandleActionIncrease);
		
		if (!m_sActionDecrease.IsEmpty())
			GetGame().GetInputManager().RemoveActionListener(m_sActionDecrease, EActionTrigger.VALUE, HandleActionDecrease);
	}
	
	//------------------------------------------------------------------------------------------------
	float GetTargetValue()
	{
		return m_fTargetValue;
	}
}