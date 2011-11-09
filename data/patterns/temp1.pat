PATTERNS:
	im = up_1 down_2 up_3 down_4 up_5 | im cr im cr im @ 
        up_1: true,
		up_3: min(up_3) > min(up_1) and max(up_3) > max(up_1),
        up_5: min(up_5) > min(up_3) and max(up_5) > max(up_3);
        
	cr =  (down_1 up_1 down_2 | im_1 cr_2 im_3) [?] @ 
        down_1: isprev(im) and max(prev()) >= max(self()) and 
                  min(prev()) <= min(self()) and
                  value(self()) <= value(prev()),
        im_1:   isprev(im) and max(prev()) >= max(self()) and 
                  min(prev()) <= min(self()) and
                  value(self()) <= value(prev());
		
	im = down_1   up_1   down_2   up_2   down_3 @ 
        down_1: true,
        down_2: min(down_2) < min(down_1) and max(down_2) < max(down_1),
        down_3: min(down_3) < min(down_2) and max(down_3) < max(down_2);

	cr = (up_1 down_1 up_2 | im_1 cr_2 im_3) [?] @ 
        up_1: isprev(im) and min(prev()) <= min(self()) and 
                max(prev()) >= max(self()) and
                value(self()) <= value(prev()),
        im_1: isprev(im) and min(prev()) <= min(self()) and 
                max(prev()) >= max(self()) and
                value(self()) <= value(prev());
                