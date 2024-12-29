/*
 * robot3_project.cpp
 *
 * Created: 2024-06-22 오전 3:48:43
 * Author : jhpark
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h> // 문자열 반환

#define UBRR 103 //16MHz -> Baud rate = 9600, U2X = 0

volatile int g_cnt = 0; // 타이머 관련 전역변수 설정

//unsigned char ndata;

// 서보모터 angle 값 전역변수로 설정
volatile double angle;

// 센서값 초기화 - 전역변수로 설정
unsigned int potentiometer_res = 0;
unsigned int cds_res = 0;
unsigned int thermister_res = 0;
unsigned int psd_res = 0;
unsigned int sound_res = 0;
unsigned int water_res = 0;

// 전역 변수로 pi 선언 및 초기화
const double pi = 3.14159265;


//---------------------------------필터 전역변수 선언----------------------------
// First order RC LPF filter
volatile double LPF_fc = 5;
volatile double LPF_tau = 1/(2*LPF_fc*pi);
volatile double LPF = 0;
volatile double LPF_past = 0;
volatile double dt = 0.01;

// Moving Average Filter
volatile double MAF_thermister[10] = {0,};
volatile int m_t = 0;
volatile double MAF_t = 0;

// IIR LPF paremeter filter order=2, Fc=0.2
volatile double b_iir[3] = {0.00003913, 0.00007826, 0.00003913};
volatile double a_iir[3] = {1, -1.9822289, 0.9823854};
volatile double IIR_input[3] = {0,};
volatile double IIR[3] = {0,};
	
// FIR LPF paramenter filter order=30, Fc=3
volatile double FIR_input_30[31]={0,};
volatile double FIR = 0;
volatile double b_fir_30[31] = {0.00066, 0.0012, 0.0023, 0.0043, 0.0074, 0.0118, 
	0.0175, 0.0245, 0.0326, 0.0412, 0.0499, 0.0581, 0.0653, 0.0708, 0.0743, 0.0755, 0.0743, 0.0708,0.0653, 
	 0.0581, 0.0499, 0.0412, 0.0326, 0.0246, 0.0176, 0.0118, 0.0074, 0.0043, 0.00235, 0.00124, 0.00066};

// IIR LPF paremeter filter order=2, Fc=3
volatile double b_iir_psd[3] = {0.0078, 0.01564, 0.0078};
volatile double a_iir_psd[3] = {1, -1.7347, 0.7660};
volatile double IIR_input_psd[3] = {0,};
volatile double IIR_psd[3] = {0,};
	
// FIR LPF paramenter filter order=10, Fc=2
volatile double FIR_input_10_water[11] = {1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023};
volatile double FIR_water = 0;
volatile double b_fir_10_water[11] = {0.0138,0.0297,0.0717,0.12458,0.1679,0.18457,0.1679,0.1246,0.0717,0.0297, 0.0138};


//----------------------------------------필터 함수 선언--------------------------------
double Low_Pass_Filter(double data)
{
	LPF = (dt*data+LPF_tau*LPF_past)/(LPF_tau+dt); //RC필터, dt=0.01
	LPF_past = LPF;	//이전 값 저장
	
	return LPF; 
}

double Moving_Average_Filter_thermister(double data)
{
	
	MAF_t=0;
	MAF_thermister[m_t] = data; // 입력 데이터 저장
	int j = 0;
	for(j=0;j<10;j++)
	{
		MAF_t += MAF_thermister[j]; // 샘플링 데이터 합 저장
	}
	if (m_t == 9) m_t = 0;
	else m_t ++;
	
	return (MAF_t/10); // 10개의 데이터의 평균을 반환
}

double IIR_LPF_cds(double data)
{	
    // 새로운 입력 데이터를 배열의 첫 번째 위치에 저장하고 나머지 값을 한 칸씩 뒤로 이동
    for (int i = 2; i > 0; i--) {
	    IIR_input[i] = IIR_input[i - 1];
	    IIR[i] = IIR[i - 1];
    }
    IIR_input[0] = data;

    // IIR 필터 계산
    IIR[0] = b_iir[0] * IIR_input[0] +
    b_iir[1] * IIR_input[1] +
    b_iir[2] * IIR_input[2] -
    a_iir[1] * IIR[1] -
    a_iir[2] * IIR[2];

    return IIR[0]; // 필터링된 값을 반환
}

double FIR_LPF_30(double data)
{
    int i = 0;
    // 새로운 입력 데이터를 배열의 첫 번째 위치에 저장하고 나머지 값을 한 칸씩 뒤로 이동
    for (i = 30; i > 0; i--) {
	    FIR_input_30[i] = FIR_input_30[i - 1];
    }
    FIR_input_30[0] = data;
    // FIR Filtering function
    FIR = 0;
    for (i = 0; i < 31; i++) {
	    FIR += b_fir_30[i] * FIR_input_30[i];
    }
    return FIR;
}

double IIR_LPF_psd(double data)
{
    // 새로운 입력 데이터를 배열의 첫 번째 위치에 저장하고 나머지 값을 한 칸씩 뒤로 이동
    for (int i = 2; i > 0; i--) {
	    IIR_input_psd[i] = IIR_input_psd[i - 1];
	    IIR_psd[i] = IIR_psd[i - 1];
    }
    IIR_input_psd[0] = data;

    // IIR 필터 계산
    IIR_psd[0] = b_iir_psd[0] * IIR_input_psd[0] +
    b_iir_psd[1] * IIR_input_psd[1] +
    b_iir_psd[2] * IIR_input_psd[2] -
    a_iir_psd[1] * IIR_psd[1] -
    a_iir_psd[2] * IIR_psd[2];

    return IIR_psd[0]; // 필터링된 값을 반환
}

double FIR_LPF_10_water(double data)
{
    int i = 0;
    // 새로운 입력 데이터를 배열의 첫 번째 위치에 저장하고 나머지 값을 한 칸씩 뒤로 이동
    for (i = 10; i > 0; i--) {
	    FIR_input_10_water[i] = FIR_input_10_water[i - 1];
    }
    FIR_input_10_water[0] = data;
    // FIR Filtering function
    FIR_water = 0;
    for (i = 0; i < 11; i++) {
	    FIR_water += b_fir_10_water[i] * FIR_input_10_water[i];
    }
    return FIR_water;
}

//ECHO 실습코드
void USART0_TX(unsigned char data)
{
	while(!(UCSR0A & (1<<UDRE0))); // UDRE = 1 -> buffer empty -> ready to write
	UDR0 = data;
}

unsigned char USART0_RX(void)
{
	while(!(UCSR0A & (1<<RXC0))); // 데이터를 수신될 때까지 기다리기 위해, 즉 RXC0 비트가 1이 될 떄까지 기다리기 위해 사용
	return UDR0;
}

void USART0_NUM(unsigned int nNum) // 정수값 아스키 코드값으로 사용
{
	USART0_TX(nNum / 10000 + 48);				//10000의 자리
	USART0_TX((nNum % 10000) / 1000 + 48);		//1000의 자리
	USART0_TX((nNum % 1000) / 100 + 48);		//100의 자리
	USART0_TX((nNum % 100) / 10 + 48);			//10의 자리
	USART0_TX((nNum % 10) + 48);				//1의 자리
}

// ADC 값 읽기 함수 // ADMUX 선택하는 함수
uint16_t readADC(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF8) | (channel & 0x07); // 채널 선택
	ADCSRA |= (1 << ADSC); // 변환 시작
	while (ADCSRA & (1 << ADSC)); // 변환 완료 대기
	return ADC;
}

// 서보모터 pwm
unsigned int set_servo(double angle)
{
	// 0 : 0.5ms , 90 : 1.5ms , 180 : 2.5ms
	double width;
	double duty;
	
	width = (angle/90) + 0.5;
	
	duty = (width / 20.0) *100 ;
	
	OCR3C = int(duty/100 *ICR3);
	return OCR3C;
}

// 가변저항
double get_Potentiometer(void)
{
	uint16_t adc = readADC(0); // ADC0 활성화
	
	double filtered_adc = Low_Pass_Filter(adc); // LPF로 ADC값 필터링
	double Vadc = (double)filtered_adc * 5.0 / 1023.0; // ADC값 전압으로 변환
	int res = (int)Vadc;
	
	//USART0_NUM(adc);
	//USART0_TX(44); // ',' 문자
	//USART0_TX(13);
	
	return res;
}

// 조도센서
double get_cds(void)
{
	uint16_t adc = readADC(1); // ADC1 활성화
	adc = IIR_LPF_cds(adc); // IIR로 필터링
	double Vadc = (double)adc * 5.0 / 1023.0; // ADC값 전압으로 변환
	
	//------------수치화---------------
	int R = 4700;
	double R_cds = (R*5)/Vadc - R; // CDS의 저항값
	double gamma = 0.8;
	double lux = pow(10,1-(log10(R_cds)-log10(40000))/gamma); // LUX단위로 변환
	
	int res = (int)lux;

	//USART0_NUM(adc);
	//USART0_TX(44); // ',' 문자
	//USART0_TX(13);
	
	return res;
}

// 온도센서
double get_Thermistor()
{
	uint16_t adc = readADC(3); // ADC3 활성화
	adc = Moving_Average_Filter_thermister(adc); // MAF로 ADC값 필터링
	double Vadc = adc * 5.0 / 1024.0;
	
	//-----수치변환---------------------
	double Rth = (5.0/Vadc) * 4700 - 4700;
	double T = 1/ (0.0033 + (0.000274)*log(Rth/1000)); //[K]
	T = T - 273.15; //[C]
	
	int res = (int)T;
	
	//USART0_NUM(T);
	//USART0_TX(44); // ',' 문자
	//USART0_TX(13);
	
	return res;
}

// PSD센서
double get_PSD()
{
	uint16_t adc = readADC(4); // ADC4 활성화
	adc = IIR_LPF_psd(adc); // IIR로 필터링
	double Vadc = adc * 5.0 / 1024.0; // ADC값 전압으로 변환
	
	//-----수치변환---------------------
	double dist = (27.61)/(Vadc * 1000.0 - 0.1696)*1000.0;
	
	
	//USART0_NUM(dist);
	//USART0_TX(44); // ',' 문자
	//USART0_TX(13);
	
	//int res = (int)dist;
	
	//return res;
	return dist;
}

// 사운드 센서
double get_sound()
{
	uint16_t adc = readADC(5); // ADC5 활성화
	double filtered_adc = FIR_LPF_30(adc);
	
	//USART0_NUM(filtered_adc);
	//USART0_TX(44); // ',' 문자
	//USART0_TX(13);
	
	int res = (int)filtered_adc;
	
	return res;
}

// 빗물 감지 센서
double get_water()
{
	uint16_t adc = readADC(6); // ADC6 활성화
	double filtered_adc = FIR_LPF_10_water(adc); // FIR 10차로 필터링
	
	//USART0_NUM(adc);
	//USART0_TX(44); // ',' 문자
	//USART0_TX(13);
	//return Vadc;
	int res = (int)filtered_adc;
	
	return res;
}


ISR(TIMER2_OVF_vect)
{
	g_cnt++;
	
	// 전체 선세의 값을 제어 주기를 통해 받아옴
	potentiometer_res = get_Potentiometer();
	cds_res = get_cds();
	thermister_res = get_Thermistor();
	psd_res = get_PSD();
	sound_res = get_sound();
	water_res = get_water();

	if(g_cnt == 100) //1000ms
	{
		g_cnt = 0;
		
		//double adc_potentiometer = get_Potentiometer();
		//double adc_cds = get_cds();
		//double adc_thermistor = get_Thermistor();
		//double adc_psd = get_PSD();
		//double adc_sound = get_sound();
		
		//USART0_NUM(adc_potentiometer); USART0_TX(44);
		//USART0_NUM(adc_cds); USART0_TX(44);
		//USART0_NUM(adc_thermistor); USART0_TX(44); //',' 표시
		//USART0_NUM(adc_psd); USART0_TX(44); 
		//USART0_NUM(adc_sound); USART0_TX(44); //',' 표시
		
		//USART0_TX(13);	// 출력할 때 enter 출력
	}
	
    // 타이머 초기값 재설정
	TCNT2 = 256-156;  // 1ms마다 오버플로우 발생을 유지하기 위해 다시 설정
}

int main(void)
{
	DDRA = 0xFF;
	DDRB = (1<< PB4) | (1 << PB5) | (1 << PB6);
	DDRE = (1 << PE3) | (1 << PE4) | (1 << PE5);

	
	PORTA = 0xFF;
	PORTB = (0 << PB5) | (0 << PB6);
	PORTE = 0xFF; // RGB led 초기상태 off
	//timer0_init(); // 타이머 0 초기화
	
	// -----------------------ADC---------------
	DDRF = 0x00;
	ADCSRA = 0b10000111; // 분주비가 작을수록 오차가 큼
	
	//--------------------UART--------------------
	//BAUD = 16000000/(16*(UBRR+1)) => UBRR= 103 : 9600bps
	UBRR0L = (unsigned char)UBRR;
	UBRR0H = (unsigned char)(UBRR>>8);
	
	UCSR0A = 0x00;						//현재상태 : 0으로 초기화
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);		//송수신 허가
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);	//비동기, non-parity mode, stop bit : 1bit, data : 8bit--> 나머지는 디폴트 0 이여서 얘만 1로 표시

	//---------------ITMER0----------------
	TCCR0 |= (1 << WGM00) | (1 << WGM01); // Fast PWM mode
	TCCR0 |= (1 << COM01); // Clear OC0 on compare match, set OC0 at BOTTOM (non-inverted mode)
	TCCR0 |= (1 << CS02) | (1 << CS01) | (0 << CS00); // Prescaler set to 256
	//(16000000)/(prescaler*(255+1)=(16000000/(256*256)=244.14[Hz] --> 3옥타브 시 (음계)

	//---------------TIMER1---------------- 
	// compare match, Fast PWM, TOP : ICRn, prescaler = 64 => 20ms
	TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1) | (0 << COM1B0) | (1 << WGM11) | (0 << WGM10);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (1 << CS10); // 0b 00011010 //주기 20ms 64 prescale
	// (16000000/pre)/((top+1)-tcnt) = 50[Hz]
	// (16000000/64)/((ICR1+1)-0) = 50[Hz]
	ICR1 = 4999; // top값 설정
	
	//--------------TIMER2-----------------제어주기로 사용
	//TIMER2, normal Mode, non-inverting mode, prescaler = 1024
	TCCR2 = (0 << FOC2) | (0 << WGM21) | (0 << WGM20) | (1 << COM21) | (0 << COM20) | (1 << CS22) | (0 << CS21) | (1 << CS20);
	// TOIE2 = 1 : OverFlow Interrupt Enable
	TIMSK = (1<<TOIE2);
	// (16000000/pre)/((top+1)-tcnt) = 100[Hz] 주기 10ms 타이머
	// (16000000/1024)/((255+1)-tcnt) = 100[Hz]
	// 15625/100 = 256 - tcnt
	TCNT2 = 256-156;
	
	//---------------TIMER3----------------
	// compare match, Fast PWM, TOP : ICRn, prescaler = 64 => 20ms
	TCCR3A = (1 << COM3A1) | (0 << COM3A0) | (1 << COM3B1) | (0 << COM3B0) | (1 << COM3C1) | (0 << COM3C0) | (1 << WGM31) | (0 << WGM30);
	TCCR3B = (1 << WGM33) | (1 << WGM32) | (0 << CS32) | (1 << CS31) | (1 << CS30); // 0b 00011010 //주기 20ms 64 prescale
	// (16000000/pre)/((top+1)-tcnt) = 50[Hz]
	// (16000000/64)/((ICR1+1)-0) = 50[Hz]
	ICR3 = 4999; // top값 설정
	
	sei();
	
	while(1)
	{
		//--------------Potentiometer---------------- LED색 변경
		//가변저항의 ADC값을 해당하는 전압으로 변환
		//TCCR0의 OCR0을 조절하여 타이머0의 펄스폭 변조를 통해 led 밝기 조절
			//--> 저항값 커지면 밝기 세짐, 저항 작아지면 밝기 작아짐
		if (potentiometer_res >= 1) {
			switch(potentiometer_res)
			{
				case 0:
					OCR3A = 0;
					break;
				case 1:
					OCR3A = 10;
					OCR3B = 4500;
					break;
				case 2:
					OCR3A = 2000;
					OCR3B = 4500;
					break;
				case 3:
					OCR3A = 3000;
					OCR3B = 4500;
					break;
				case 4:
					OCR3A = 4000;
					OCR3B = 4500;
					break;
				case 5:
					OCR3A = 4999;
					OCR3B = 4500;
					break;
				default:
					break;
			}
		}
		else
		{
			// -----------------CDS---------------------- LED 밝기 변경
			// 센서의 Analog Output을 Lux단위로 값을 변환
			// 주변의 밝기에 따라 효율적으로 LED PWM 조절
			if(cds_res <= 150){
				OCR3B = 4500; // 어두운 경우
			}
			else if(cds_res >= 150 && cds_res <= 400){
				OCR3B = 3000; // 빛이 많이 없는 경우
			}
			else if(cds_res >= 400 && cds_res <= 800){
				OCR3B = 1500; // 오전
			}
			else if(cds_res >= 800){ 
				OCR3B = 0; // 빛을 많이 감지한 경우
			}
		}
		
		
		//---------------------------Thermister----------------- dc모터
		// 센서의 analog output을 섭씨온도단위로 값을 변환
		// 온도가 올라가면서 PWM duty ratio를 상승시켜 모터의 속도를 증가 // OCR1A 사용 
		if(thermister_res <= 28) OCR1A = 0;
		else if(thermister_res == 29) OCR1A = ICR1*(0.2);
		else if(thermister_res == 30) OCR1A = ICR1*(0.4);
		else if(thermister_res == 31) OCR1A = ICR1*(0.6);
		else if(thermister_res == 32) OCR1A = ICR1*(0.8);
		else if(thermister_res == 33) OCR1A = ICR1;
		else OCR1A = ICR1;
		
		
		//-------------------------PSD------------------------ 서보모터
		if(psd_res >= 25) angle = 110.0;
		else if(psd_res >= 20 && psd_res < 25) angle = 90.0;
		else if(psd_res >= 15 && psd_res < 20) angle = 60.0;
		else if(psd_res >= 11 && psd_res < 15) angle = 30.0;
		else if(psd_res < 11) angle = 10.0;
		else angle = -10.0;
		//set_servo 함수 안에서 OCR3C 사용
		set_servo(angle);
		
		
		//---------------------Sound Sensor---------------------- 수동형 부저 모듈
		// 센서의 Analog Output을 dB단위로 값을 변환
		// 온도가 올라가면서 OCR0의 값을 증가시키면서 부저모듈의 PWM 증가 ( 소리 커짐 ) // OCR1B 사용
		if(sound_res <= 100) {
			 ICR1 = 7645;		//1옥타브 도
			 OCR1B = ICR1-1;	// duty 100% 소리 안남
		}
		else if(sound_res > 100 && sound_res <= 150) {
			ICR1 = 3822;	//2옥타브 도
			OCR1B = ICR1/2;
		}
		else if(sound_res > 150 && sound_res <= 200) {
			ICR1 = 1911;	//3옥타브 도
			OCR1B = ICR1/2;
		}
		else if(sound_res > 200 && sound_res <= 250) 
		{
			ICR1 = 955;	//4옥타브 도
			OCR1B = ICR1/2;
		}
		else if(sound_res > 250 && sound_res <= 300) {
			ICR1 = 477;	//5옥타브 도
			OCR1B = ICR1/2;
		}
		else if(sound_res > 300 && sound_res <= 500) {
			ICR1 = 238;	//6옥타브 도
			OCR1B = ICR1/2;
		}
		else 
		{
			ICR1 =238;
			OCR1B = ICR1/2;
		}
		
		//---------------------Water Sensor------------------- 진동 모터 모듈 
		// 센서의 Analog Output을 adc값으로 받아옴	//OCR0 사용
		// 물이 없을 때 1023 물을 가장많이 인식했을 때 200초반대
		if(water_res >= 900) OCR0 = 0;
		else if(water_res < 900 && water_res >= 600) OCR0 = 50;
		else if(water_res < 600 && water_res >= 400) OCR0 = 150;
		else if(water_res < 400 && water_res >= 300) OCR0 = 200;
		else if(water_res < 300 && water_res >= 100) OCR0 = 255;
		else OCR0 = 255;		
	}
}
