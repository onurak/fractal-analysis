PATTERNS:
	IdealImpulse = a_1 b_2 a_3 b_4 a_5 @
		max(a_3) > max(a_1) and 
		max(a_5) > max(a_3) and 
		min(a_5) > max(a_1) and
		(value(a_3) > value(a_1) or value(a_3) > value(a_1));
		
	WrongImpulse = a_1 b_2 a_3 b_4 a_5 @
		max(a_3) > max(a_1) and 
		max(a_5) > max(a_3) and 
		min(a_5) > max(a_1) and
		value(a_3) < value(a_1);
		
	Test = IdealImpulse @ true;
	Complete = WrongImpulse @ true;