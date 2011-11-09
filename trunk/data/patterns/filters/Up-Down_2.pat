PATTERNS:
	up = a @ true;
	down = b @ true;

	up = up_1 down_2 up_3 @
		value(down_2) < value(up_1  )*0.2 and value(down_2) < value(up_3  )*0.2;
	down = down_1 up_2 down_3 @
		value(up_2  ) < value(down_1)*0.2 and value(up_2  ) < value(down_3)*0.2;
	
	# up   = a | up up @ true;
	# down = b | down down @ true;
	