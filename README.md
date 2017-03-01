# GPUPrograming
ITESO GPU programming Hwks and reads


Vertical operations and horizontal operations:



Application:

Point product between two vectors.

With the vertical product ans horizontal sum.

V0=[1,2,3,4]
V1=[4,3,2,1]
Dot(V0,V1) = 20
C<-V0*V1= [4,6,6,4] <- vertical operation
C := C h+ C // apply horizontal sumation
[10,10,10,10]

- - -
V0=[1,2,3,4]
V1=[5,6,7,8]
Dot(V0,V1) = 70
C<-V0*V1= [5,12,21,32] <- vertical operation
C := C h+ C // apply horizontal sumation
[5,12,21,32]
[5+12,21+32,5+12,32+21]
[17, 53, 17, 53]
[70,70,70,70]


Visual studio
debug flag: `_DEBUG`
Cambia cuando se cambia el modo de release a debug


Group is the minimum unit of computation for directx.
Every unit is a cube with 3 dimensions
Grupo: Equipo de hilos que comparten  variables globales.

The cpu starts cubes in the GPU.

[https://msdn.microsoft.com/en-us/library/windows/desktop/ff476405(v=vs.85).aspx](https://msdn.microsoft.com/en-us/library/windows/desktop/ff476405(v=vs.85).aspx)
```
int x; // this variable is only visible for this Group
// define the number and structure of the Group
[numthreads(1,1,1)]
void main()
{

}
```


## CPU vs GPU

| CPU | GPU     |
| :------------- | :------------- |
| Low latency memory       | high bandwidth memory       |
| random accesses | sequential accesses |
| 0.1 Tflop compute | 1Tflop computing |
| 1GFlop/watt | 10GFlog/watt |


The CPU is designed to handle random access operations.





### _The PC as an asymetric Multiprocessor system_


## Structured buffers

context & factory


```
StructuredBuffer Input;
RWStructuredBuffer Output; /* Read write StructuredBuffer */

int x;

[numthreads(10,8,3)]

void main(uint3 id:SV_DispatchThreadID)
{

}
```


Intrinsic types, son parte intrinsica de del lenguaje.


## Constant Buffers


    Constant buffer:
    Es una regrion de memoria interna en el GPU para parametrizar el funcionamiento de los compute shaders, antes de que estos se inicien.

    Una vez iniciada la ejecucion de un compute shader, esas varaibles son consideradas de solo lectura

    La ventaja de los constant buffers es que el cpu puede actualizar el valor de estas variables en cualquier momento y de manera eficiente.

    1) Crear un buffer con las siguientes caracteristicas
    	D3D11_BIND_CONSTANTBUFFER,
    	D3D11_CPU_ACCESS_WRITE
    		El tamaño de un constatn buffer debe ser multiplo de 16 bytes.
    2) No se requiere vista para un constant buffer.
    3) El formato del contendio queda definido por el programador

    Aplicaciones:
    Paso de matrices, paso de parametros simples, valores iniciales, iteraciones maximas, etc.
    El dispatcher solo da como parametros el thread id
