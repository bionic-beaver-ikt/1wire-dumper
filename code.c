	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(480);
	THERM_INPUT_MODE();
	_delay_us(60);
	i=(THERM_PIN & (1<<THERM_DQ));
	_delay_us(420);
	
	THERM_LOW();
	THERM_INPUT_MODE();
	while (THERM_PIN&(1<<THERM_DQ)) {};
	_delay_us(480);
	for (i=0; i<480; i++)
	{
		if(THERM_PIN&(1<<THERM_DQ))
		{
			i=480;
			_delay_us(55);
			THERM_OUTPUT_MODE();
			_delay_us(235);
			THERM_INPUT_MODE();
			_delay_us(190);
			while (THERM_PIN&(1<<THERM_DQ)) {};
			int command[32];
			for (j=0; j<32; j++)
			{
				for (k=0; k<120; k++)
				{
					if(THERM_PIN&(1<<THERM_DQ))
					{
						if (k<15) command[j] = 1; else command[j] = 0;
						k=120;
					}
				_delay_us(1);
				}
			}
		}
	_delay_us(1);
	}


	}
	_delay_us(1);
	}

	
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(1);
	if(bit) THERM_INPUT_MODE();
	_delay_us(60);
	THERM_INPUT_MODE();
	
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(1);
	THERM_INPUT_MODE();
	_delay_us(14);
	if(THERM_PIN&(1<<THERM_DQ)) bit=1;
	_delay_us(45);