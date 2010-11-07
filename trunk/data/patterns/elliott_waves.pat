####### Elliott Wave Base ########
# First version of patterns for Elliott wave theory.
#
#*** PATTERN LIST ****
# Impulses:
#	 im - low level impulse (syn. LT, ET)
#  	 LT - leading triangle (syn. im, ET)
#	 ET - ending triangle (syn. im, LT)
#
# Corrections:
# 	 cr - low level correction
#	 FL - flat (include simple flat, irregular flat, expanded flat)
#    ZZ - zigzag
#    DZ - double zigzag
#    TZ - triple zigzag
#	 D3 - double trees
#	 T3 - triple trees

# Leading Triangle
LT =
	im cr im cr im @ true;

# Ending Triangle
ET =
	cr cr cr cr cr @ true;
	
# Flat, Irregular Flat, Expanded Flat
FL = 
	(cr_1 | ZZ_1 | DZ_1 | TZ_1 | FL_1 | D3_1 | T3_1) 
	(cr_2 | ZZ_2 | DZ_2 | TZ_2 | FL_2 | D3_2 | T3_2) 
	(im_3 | ET_3)
	@	value(*_2, min) >= value(*_1, max) and
		min(*_3) < max(*_1) and
		value(*_3, min) >= value(*_2, max);
	
	
# ZigZag
ZZ =
	(im_1 | LT_1) 
	cr_2
	(im_3 | ET_3)
	
	
	@	(value(cr_2) < value(*_1, min)) and 
		(value(*_3, min) >= value(cr_2, 0.7));		
	
# Double ZigZag
DZ =
	ZZ_1 
	(cr_2 | ZZ_2 | DZ_2 | TZ_2 | FL_2 | CT_2 | ET_2 | D3_2 | T3_2) 
	ZZ_3
	@	(value(*_2, max) <  value(ZZ_1)) and
		(value(*_2, max) <= value(ZZ_3));
	
# Triple ZizZag
TZ =
	(ZZ_1) 
	(cr_2 | ZZ_2 | DZ_2 | TZ_2 | FL_2 | ET_2 | D3_2 | T3_2) 
	(ZZ_3) 
	(cr_4 | ZZ_4 | DZ_4 | TZ_4 | FL_4 | ET_4 | D3_4 | T3_4)
	(ZZ_5)
	@	(value(*_2, max) <  value(ZZ_1)) and
		(value(*_2, max) <= value(ZZ_3)) and
		(value(*_4, max) <  value(ZZ_3)) and
		(value(*_4, max) <= value(ZZ_5));	

# Double Trees
D3 = 
	(cr_1 | D3_1 | T3_1)
	(cr_2 | D3_2 | T3_2)
	(cr_3 | D3_3 | T3_3)
	@	value(*_2) >= value(*_1, 0.5) and
		value(*_3) >= value(*_1, 2);

# Triple Trees
T3 = 
	(cr_1 | D3_1 | T3_1)
	(cr_2 | D3_2 | T3_2)
	(cr_3 | D3_3 | T3_3)
	(cr_4 | D3_4 | T3_4)
	(cr_5 | D3_5 | T3_5)
	@	value(*_2) >= value(*_1, 0.5) and
		value(*_3) >= value(*_1, 2) and
		value(*_4) >= value(*_3, 0.5) and
		value(*_5) >  value(*_4);
		
# impulse
im =
	a_1 b_1 a_2 b_2 a_3
	@ min(a_2) > min(a_1) and min(a_3) > min(a_2) and max(a_3) > max(a_2);

# impulse
im =
	b_1 a_1 b_2 a_2 b_3
	@ max(b_2) < max(b_1) and max(b_3) < max(b_2) and min(b_3) < min(b_2);

# impulse
im =
	im_1 cr_1 im_2 cr_2 im_3
	@ min(im_2) > min(im_1) and min(im_3) > min(im_2);

# impulse
im =
	cr_1 im_1 cr_2 im_2 cr_3
	@ max(cr_2) < max(cr_1) and max(cr_3) < max(cr_2) and min(cr_3) < min(cr_2);

# correction
cr =
	b a b | cr im cr | a b a | im cr im
	@ (previous(im) == 1);
	
# Synonyms Table
synonyms:
	im = LT,
	im = ET;