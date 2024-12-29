clc                          % 저장된 변수나 함수를 제거하는 코드
clear all                    % command window를 제거하는 코드
close all                    % 모든 process를 제거하는 코드

%% Set Parameters
    %Set Simulation Time
        end_time = 10;           % 그리고자 하는 그래프를 측정할 시간을 선택
        delta_t = 0.02;        % 0.001초마다 값을 체크하여 값을 찍겠다는 의미

%% Read Data File
    fileID = fopen('thermistor_adc.txt', 'r');
    y = fscanf(fileID, '%d,');  % 쉼표로 구분된 정수 읽기
    fclose(fileID);

    n=1;
    m=1;
    for(t=0:delta_t:end_time)
    
        sim_y(n) = y(m);
        sim_time(n) = t;
        n=n+1;
        m=m+2;
    end

%% Moving Average Filter
size = 8; % 평균낼 데이터의 사이즈를 조절
n = size;

% 필터의 코드는 강의자료 ppt를 참고하여 작성하였습니다.
for(i = size*delta_t : delta_t : end_time)
    Avg = 0; % for 문을 한번 실행할 때마다 새로운 Avg값을 넣어주어야 하므로 초기화
    
    % simulation
    for(i = 0 : 1 : size-1)
        MAF_BUFF(i+1) = sim_y(n-i); % size만큼의 데이터를 더해서 배열에 집어넣음
    end
    
    L = length(MAF_BUFF);
    Avg = sum(MAF_BUFF)/L; % MAF_BUFF의 길이를 측정 후 더해진 size만큼의 데이터를 평균내기 위해 길이로 나누어줌.
    
    n = n + 1;      %  n의 값을 1씩 증가시키면서 전체 데이터를 이동하면서 size만큼 평균냄
    maf(n) = Avg;   % for 문을 한 번씩 수행할 때마다 maf 배열에 평균낸 값을 집어넣음.
end
maf(1:10)=848;
%% LPF RC filter
cutoff = 1.5;     % 차단 주파수를 설정함
tau = 1 / (2*pi*cutoff);    % 시정수를 계산함
lpf(1) = 848;      % 매트랩의 배열 시작은 1부터 이므로 초기화
n = 2;           %  n-1을 계산하기 때문에 2부터 시작하도록 설정

% get data
% 강의자료 ppt의 계산방법을 참고하여 작성하였습니다.
for t = delta_t : delta_t : end_time 
    lpf(n) = ((delta_t * sim_y(n)) + (tau * lpf(n-1))) / (tau + delta_t);
    n = n + 1;
end


%% Calc FFT
    % Set FFT
        Fs              = 1/delta_t;        % Sampling Frequency
        T               = delta_t;          % Sampling Period
        L1              = length(sim_y);   % Length of each Signal
        L2              = length(lpf);
        L3              = length(maf);

        T_vector        = (0:L1-1)*T;        % TIme Vector of signal

        fft_f1           = Fs*(0:(L1/2))/L1;    % Frequency Range of each signal
        fft_f2           = Fs*(0:(L2/2))/L2;    % 분석 주파수 분해능 : fft_f(2) - fft_f(1)
        fft_f3           = Fs*(0:(L3/2))/L3;

      %Calc FFT
        fft_y_temp1      = abs(fft(sim_y)/L1);        % 허수부를 제거 
        fft_y_temp2      = abs(fft(lpf)/L2);
        fft_y_temp3      = abs(fft(maf)/L3);

        fft_y1           = fft_y_temp1(1:floor(L1/2)+1);            % 켤레 복소수 대응
        fft_y2           = fft_y_temp2(1:floor(L2/2)+1);            % 컴파일은 잘 되었지만, '인덱스로 사용시, 콜론 연산자에는 정수형 피연산자가 필요하다'고 경고 문구가 떠서 floor 함수를 사용하여 소수점 뒤자리를 삭제하고 정수로 변형하였습니다. 
        fft_y3           = fft_y_temp3(1:floor(L3/2)+1); 
       
        fft_y1(2:end-1)  = 2*fft_y1(2:end-1);         % 켤레 복소수 대응 
        fft_y2(2:end-1)  = 2*fft_y2(2:end-1);
        fft_y3(2:end-1)  = 2*fft_y3(2:end-1);


 %% FIR Low-pass Filter_filtfilt()

     n = 2;                             % filter order
     Fc = 5;                            % cutoff frequency
     Fs = 1/delta_t;                    % sampling frequency
     Fn = Fs/2;                         % nyquist frequency
     Wn = Fc/Fn;                        
    
     b = fir1(n, Wn, 'low');            % design FIR LPF
     sim_y3 = filtfilt(b, 1, sim_y);    % output result


%% Draw Graph
    % Draw Time Domain Graph
    figure('units', 'pixels', 'pos', [100 100 700 600], 'Color',[1, 1, 1]);
    plot(sim_time,sim_y,'-k','LineWidth',2)                        % linewidth는 라인의 두께를 의미, -k는 라인의 solid, 검정색 옵션을 나타냄, 앞의 두 매개변수는 x, y좌표를 의미
    hold on     % 한 figure에 여러개의 그래프를 나타내고 싶을 때 사용함
    plot(sim_time,lpf,'-r','LineWidth',2)
    hold on
    plot(sim_time,maf,'-b','LineWidth',2)
    legend('Sensor value','LPF','MAF')            % legend 함수를 이용하여 각 그래프의 이름을 우측 상단에 표시함 ( 범례 표시 )
    grid on
    
    xlabel('Time(s)','FontSize', 20);
    ylabel('Magnitude','FontSize', 20);
    title('sensor value', 'FontSize', 20);


    % Draw Frequency Damain Graph
    figure('units', 'pixels', 'pos', [800 100 700 600], 'Color',[1, 1, 1]);     
    % figure로 새 창 생성 % [1,1,1] 배경 흰색을 의미 % Xpos, Ypos는 figure창의 픽셀 위치를 
    % Xsize, Ysize는 figure창의 크기를 의미 % units는 측정 단위의 값으로 pixel을 가짐.
        subplot(3,1,1) % Frequency-Domain
    
                % stem(fft_f1,fft_y1,'-k','LineWidth',2)
                stem(fft_f1(1:50),fft_y1(1:50),'-k','LineWidth',2)
                grid on;                          
                % ylim([0 50]); % y축 한계를 0에서 100으로 설정
            xlabel('Frequency (Hz)',      'FontSize',20);
            ylabel('Magnitude',           'FontSize',20);
            title('Frequency Domain',     'FontSize',25);
        
        subplot(3,1,2) %Frequency-Domain
            
                % stem(fft_f2,fft_y2,'-k','LineWidth',2)
                stem(fft_f2(1:50),fft_y2(1:50),'-k','LineWidth',2)
                grid on;
                % ylim([0 50]); % y축 한계를 0에서 100으로 설정
            xlabel('Frequency (Hz)', 'fontsize',20);    
            ylabel('Magnitude',      'fontsize',20);   
            title ('LPF Filter - Frequency Domain','fontsize',25);
        
        subplot(3,1,3) %Frequency-Domain

                % stem(fft_f3,fft_y3,'-k','LineWidth',2)
                stem(fft_f3(1:50),fft_y3(1:50),'-k','LineWidth',2)
                grid on;                                 
                % ylim([0 50]); % y축 한계를 0에서 100으로 설정
           xlabel('Frequency (Hz)', 'fontsize',20);    
           ylabel('Magnitude',      'fontsize',20);   
           title ('MAF Filter - Frequency Domain','fontsize',25);