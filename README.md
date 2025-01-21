# TempControllerCYD
 Um controlador e monitorador de temperatura usando um Cheap Yellow Display (CYD)

Ele lê a temperatura de um termopar com MAX6675 ou de um DS18B20 e aciona um sinal digital para ligar um aquecedor.

Usando o DS18B20, o D27 é usado como pino de dados e o D22 como saída para o aquecedor.

## Tela
A tela vai ter 3 abas: uma de controle, uma de comunicação e uma terceira de gráfico.

### Controle
A tela de controle tem um slider e 2 botões para definir o setpoint e mais 2 botões para ligar e desligar o sistema.

### Comunicação
A tela de comunicação informará a rede à qual o sistema está conectado ou servindo e o site para acesso. Pode-se também definir se está conectada a uma rede ou gerando uma rede.

### Gráfico
A tela de gráfico tem um gráfico com o setpoint e a temperatura, desde que o sistema iniciou.