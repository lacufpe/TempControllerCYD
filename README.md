# TempControllerCYD
 Um controlador e monitorador de temperatura usando um Cheap Yellow Display (CYD).

Ele lê a temperatura de um termopar com MAX6675 ou de um DS18B20 e aciona um sinal digital para ligar um aquecedor. Gera também uma página html de onde pode-se acompanhar o gráfico da temperatura e baixar os dados coletados.

 Usando o DS18B20, o D27 é usado como pino de dados e o D22 como saída para o aquecedor.

## Tela
A tela tem 3 abas: uma de *setup*, uma de comunicação e uma terceira de gráfico.

### *Setup*
A tela de *Setup* tem sliders para definir o *setpoint* de temperatura, a histerese em torno deste *setpoint*, a taxa de amostragem do sinal e 2 botões para ligar e desligar o controle do sistema. Nela também é mostrado a temperatura atual e o tempo o experimento.

### Comunicação
A tela de comunicação informará a rede à qual o sistema está conectado (ainda não implementado) ou servindo e o site para acesso (192.168.1.4, por default do ESP32). Nela há também uma *toggle-switch* onde pode-se definir se está conectada a uma rede ou gerando uma rede.

### Gráfico
A tela de gráfico tem um gráfico com o setpoint e a temperatura, dos últimos 100 pontos adquiridos.

## Hardware

O projeto é baseado num CYD, que serve como controlador, interface com rede e IHM. A ele estão conectados um relê de estado sólido de 220 V AC, 25 A, que aciona a resistência e um MAX6675 com termopar, para medição da temperatura.

## Controle

O controle é do tipo *on-off* (*bang-bang*) com histerese. Na tela de setup há um slider que controla a histerese do controle, em graus. Seja a histerese $h$ e o *setpoint* de temperatura $T_{sp}$, o controle básico com histerese chaveia o aquecedor em $T_{sp} \pm h/2$.

### Controle preditivo
Um sistema de pirólise, como vários sistemas térmicos, pode ser modelado como um sistema FOPDT - *First Order Plus Dead Time* - onde dada uma potência elétrica de entrada $P_e$ a temperatura do material $T_m$ pode ser descrita pela função de transferência:

$$
T_m(s) = \frac{K}{1+\tau s}e^{−θs}P_e(s),
$$
onde $K$ é o ganho do sistema (em Kelvin/Watt), $\tau$ é a constante de tempo térmica e $\theta$ é o tempo de atraso ou tempo morto (*dead time*).

Para um controle on-off, podemos modelar a temperatura ao longo do tempo como o somatório de respostas a um degrau em $P_e$. Esta resposta ao degrau tem a forma:
$$
T_m(t) = \begin{cases}
    &T_0,&t<\theta\\
    &T_0 + (T_F-T_0)e^{-(t-\theta)/\tau},&t\geq\theta
    \end{cases}
$$
onde $T_0$ é a temperatura inicial e $T_F$ a final.