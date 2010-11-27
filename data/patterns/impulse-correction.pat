PATTERNS:
	im = up_1   down_1   up_2   down_2   up_3 @ 
		min(up_2) > min(up_1) and min(up_3) > min(up_2) and max(up_3) > max(up_2) and max(up_2) > max(up_1);

	cr =  down_1 up down_2 @ 
		max( extractprev(im, up, 3)  ) > max(down_2);
		
	im = down_1   up_1   down_2   up_2   down_3 @ 
		min(down_2) < min(down_1) and min(down_3) < min(down_2) and max(down_3) < max(down_2) and max(down_2) < max(down_1);

	cr = up_1 down up_2 @ 
		min( extractprev(im, down, 3)  ) < min(up_2);