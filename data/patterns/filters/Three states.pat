PATTERNS:
	u = a_1 @ value(a_1) >= 40;
	u = u u @ true;
	
	d = b_1 @ value(b_1) <= -40;
	d = d d @ true;
	
	m = a_1 @ value (a_1) < 40;
	m = b_1 @ value (b_1) > 40;
	m = m m @ true;