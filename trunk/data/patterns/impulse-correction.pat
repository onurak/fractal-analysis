PATTERNS:
	im = up_1   down_1   up_2   down_2   up_3 @ 
		min(up_2) > min(up_1) and min(up_3) > min(up_2) and max(up_3) > max(up_2) and max(up_2) > max(up_1);

    im = im cr im cr im @ true;
        
	cr =  down_1 up_1 down_2 @ 
		isprev(im) and value(prev()) * 0.7 >= value(self());
		
	im = down_1   up_1   down_2   up_2   down_3 @ 
		min(down_2) < min(down_1) and min(down_3) < min(down_2) and max(down_3) < max(down_2) and max(down_2) < max(down_1);

	cr = up_1 down_1 up_2 @ 
		isprev(im) and value(prev()) * 0.7 >= value(self());
		 