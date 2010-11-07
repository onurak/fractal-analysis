PATTERNS:
	imup = up_1   down_1   up_2   down_2   up_3 @ 
		min(up_2) > min(up_1) and min(up_3) > min(up_2) and max(up_3) > max(up_2) and max(up_2) > max(up_1);

	crdown =  down_1 up down_2 @ 
		max( extractprev(imup, up, 3)  ) > max(down_2);