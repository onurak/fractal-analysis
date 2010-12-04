PATTERNS:
	#### MOTIVE WAVES ####
	
	# Comon motive
	#MTUP   = IMUP @ true;
	#MTDN = IMDN @ true;
	
	# Common impulse
	#IMUP = IMUPClassic | IMUPFailedFifth @ true;
	#IMDN = IMDNClassic | IMDNFailedFifth @ true;
	
	# Upmoving impulse
	IMUPClassic = up_1 down_2 up_3 down_4 up_5 @
		(Min(down_4) >= Max(up_1)) and
		(Value(up_3) >= Value(up_1) or Value(up_3) >= Value(up_5)) and
		(Max(up_3) >= Max(up_1)) and
		(Min(down_2) >= Min(up_1)) and
		(Max(up_5) >= Max(up_3));
	
	IMUPFailedFifth = up_1 down_2 up_3 down_4 up_5 @
		(Min(down_4) >= Max(up_1)) and
		(Value(up_3) >= Value(up_1) or Value(up_3) >= Value(up_5)) and
		(Max(up_3) >= Max(up_1)) and
		(Min(down_2) >= Min(up_1)) and
		(Max(up_5) < Max(up_3));
	
	IMUPClassic = IMUP_1 CRDN_2 IMUP_3 CRDN_4 IMUP_5 @
		(Min(CRDN_4) >= Max(IMUP_1)) and 
		(Value(IMUP_3) >= Value(IMUP_1) or Value(IMUP_3) >= Value(IMUP_5)) and
		(Max(IMUP_3) >= Max(IMUP_1)) and
		(Min(CRDN_2) >= Min(IMUP_1)) and
		(Max(IMUP_5) >= Max(IMUP_3));
		
	IMUPFailedFifth = IMUP_1 CRDN_2 IMUP_3 CRDN_4 IMUP_5 @
		(Min(CRDN_4) >= Max(IMUP_1)) and 
		(Value(IMUP_3) >= Value(IMUP_1) or Value(IMUP_3) >= Value(IMUP_5)) and
		(Max(IMUP_3) >= Max(IMUP_1)) and
		(Min(CRDN_2) >= Min(IMUP_1)) and
		(Max(IMUP_5) < Max(IMUP_3));
		
	# Downmoving impulse
	IMDNClassic = down_1 up_2 down_3 up_4 down_5 @
		(Max(up_4) <= Min(down_1)) and 
		(Value(down_3) >= Value(down_1) or Value(down_3) >= Value(down_5)) and
		(Min(down_3) < Min(down_1)) and
		(Max(up_2) < Max(down_1)) and
		(Min(down_5) <= Min(down_3));
		
	IMDNClassic = IMDN_1 CRUP_2 IMDN_3 CRUP_4 IMDN_5 @
		(Max(CRUP_4) <= Min(IMDN_1)) and 
		(Value(IMDN_3) >= Value(IMDN_1) or Value(IMDN_3) >= Value(IMDN_5)) and
		(Max(IMDN_3) >= Max(IMDN_1)) and
		(Max(CRUP_2) < Max(IMDN_1)) and
		(Min(IMDN_5) <= Min(IMDN_3));
	
	IMDNFailedFifth = down_1 up_2 down_3 up_4 down_5 @
		(Max(up_4) <= Min(down_1)) and 
		(Value(down_3) >= Value(up_1) or Value(down_3) >= Value(up_5)) and
		(Min(down_3) < Min(down_1)) and
		(Max(up_2) < Max(down_1)) and
		(Min(down_5) > Min(down_3));
		
	IMDNFailedFifth = IMDN_1 CRUP_2 IMDN_3 CRUP_4 IMDN_5 @
		(Max(CRUP_4) <= Min(IMDN_1)) and 
		(Value(IMDN_3) >= Value(IMDN_1) or Value(IMDN_3) >= Value(IMDN_5)) and
		(Max(IMDN_3) >= Max(MT_1)) and
		(Max(CRUP_2) < Max(IMDN_1)) and
		(Min(IMDN_5) > Min(IMDN_3));


    ### Ending Diagonal Triangle (EDT) ###
	EDTUP   = EDTUPLowLevel | EDTUPClassic @ true;
	EDTDN = EDTDNLowLevel | EDTDNClassic @ true;
	
	EDTUPLowLevel = 
		up_1 down_2 up_3    
		down_4 up_5 down_6
		up_7 down_8 up_9
		down_10 up_11 down_12
		up_13 down_14 up_15
		@
		AtOneLine(Min(up_1), StartTime(up_1), 
				  Min(up_7), StartTime(up_7), 
				  Min(up_13), StartTime(up_13),
				  5) and
		AtOneLine(Max(up_3), EndTime(up_3),
				  Max(up_9), EndTime(up_9),
				  Max(up_15), EndTime(up_15),
				  5);
	
	
	EDTDNLowLevel = 
		down_1 up_2 down_3    
		up_4 down_5 up_6
		down_7 up_8 down_9
		up_10 down_11 up_12
		down_13 up_14 down_15
		@
		AtOneLine(Max(down_1), StartTime(down_1), 
				  Max(down_7), StartTime(down_7), 
				  Max(down_13), StartTime(down_13),
				  5) and
		AtOneLine(Min(down_3), EndTime(down_3),
				  Min(down_9), EndTime(down_9),
				  Min(down_15), EndTime(down_15),
				  5);
				  
	
	#EDTUPClassic = 
	#	IMUP_1 CRDN_2 IMUP_3    
	#	CRDN_4 IMUP_5 CRDN_6
	#	IMUP_7 CRDN_8 IMUP_9
	#	CRDN_10 IMUP_11 CRDN_12
	#	IMUP_13 CRDN_14 IMUP_15
	#	@
	#	AtOneLine(Min(IMUP_1), StartTime(IMUP_1), 
	#			  Min(IMUP_7), StartTime(IMUP_7), 
	#			  Min(IMUP_13), StartTime(IMUP_13),
	#			  5) and
	#	AtOneLine(Max(IMUP_3), EndTime(IMUP_3),
	#			  Max(IMUP_9), EndTime(IMUP_9),
	#			  Max(IMUP_15), EndTime(IMUP_15),
	#			  5);
	
	#EDTDNClassic = 
	#	IMDN_1 CRUP_2 IMDN_3    
	#	CRUP_4 IMDN_5 CRUP_6
	#	IMDN_7 CRUP_8 IMDN_9
	#	CRUP_10 IMDN_11 CRUP_12
	#	IMDN_13 CRUP_14 IMDN_15
	#	@
	#	AtOneLine(Max(IMDN_1), StartTime(IMDN_1), 
	#			  Max(IMDN_7), StartTime(IMDN_7), 
	#			  Max(IMDN_13), StartTime(IMDN_13),
	#			  5) and
	#	AtOneLine(Min(IMDN_3), EndTime(IMDN_3),
	#			  Min(IMDN_9), EndTime(IMDN_9),
	#			  Min(IMDN_15), EndTime(IMDN_15),
	#			  5);
	
	### Leading Diagonal Triangle (TLD) ###
	
	
		
	#### CORRECTIVE WAVES ####
	CRDN = ZZSDNLowLevel @ true;
	CRUP = ZZSUPLowLevel @ true;
	
	# Single Zigzag
	ZZSDN = ZZSDNLowLevel @ true;
	ZZSUP = ZZSUPLowLevel @ true;
	
	ZZSDNLowLevel = down_1 up_2 down_3 @ 
			IsPrev(IMUP) and Min(self()) >= Max(prev()) and Max(up_2) < Max(down_1);
	ZZSUPLowLevel = up_1 down_2 up_3 @ 
			IsPrev(IMDN) and Max(self()) <= Max(prev()) and Min(down_2) > Min(up_1);
			
			
	# Double zigzag
	#ZZDDN = 
	
SYNONYMS:
	IMUP = IMUPClassic;
	IMUP = IMUPFailedFifth;
	IMDN = IMDNClassic;
	IMDN = IMDNFailedFifth;
	