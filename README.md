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

### [How the data is copied from CPU->GPU](http://stackoverflow.com/questions/15814211/when-is-the-data-copied-to-gpu-memory)

DirectX transfers data from system memory into video memory when the creation methods
are called. An example of a creation method is `ID3D11Device::CreateBuffer`. This method requires a pointer to the memory location of where the data is, so it can be copied from
system RAM to video RAM. However, if the pointer that is passed into, is a null pointer
then just sets the amount of space, so you can copy the data later.


Example:
```C
// Fill in a buffer descrition
D3D11_BUFFER_DESC bufferDesc;
bufferDesc.Usage            = D3D11_USAGE_DYNAMIC;
bufferDesc.ByteWidth        = sizeof(Vertex_dynamic) * m_iHowManyVertices;
bufferDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
bufferDesc.CPUAccessFlags   = D3D11_CPU_ACCESS_WRITE;
bufferDesc.MiscFlags        = 0;
bufferDesc.StructureByteStride = NULL;

// Fill in the subresource data.
D3D11_SUBRESOURCE_DATA InitData;
InitData.pSysMem = &_vData[0];
InitData.SysMemPitch = NULL;
InitData.SysMemSlicePitch = NULL;

// Create the vertex buffer.
/*Data is being copyed right now*/
m_pDxDevice->CreateBuffer(&bufferDesc, &InitData, &m_pDxVertexBuffer_PiecePos);

```







## CPU vs GPU

| CPU | GPU     |
| :------------- | :------------- |
| Low latency memory       | high bandwidth memory       |
| random accesses | sequential accesses |
| 0.1 Tflop compute | 1Tflop computing |
| 1GFlop/watt | 10GFlog/watt |


The CPU is designed to handle random access operations.

## DirectCompute Memory Model
### register based memory

### device memory
### group shared memory




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


Intrinsic types, son parte intrinsica de del lenguaje.

###[Resouce Management Best Practices](https://msdn.microsoft.com/en-us/library/windows/desktop/ee418784(v=vs.85).aspx#managed_resources)



#### Scalar Data types
https://msdn.microsoft.com/en-us/library/windows/desktop/bb509646(v=vs.85).aspx


Tutorial:
http://www.codinglabs.net/tutorial_compute_shaders_filters.aspx

HLSL types:
https://msdn.microsoft.com/en-us/library/windows/desktop/bb509634(v=vs.85).aspx

https://software.intel.com/sites/default/files/m/d/4/1/d/8/DirectCompute_on_DirectX_11.pdf

https://code.msdn.microsoft.com/windowsdesktop/DirectCompute-Basic-Win32-7d5a7408#content


https://msdn.microsoft.com/en-us/library/windows/desktop/ff476405(v=vs.85).aspx

registers:

https://msdn.microsoft.com/en-us/library/hh447206(v=VS.85).aspx

http://on-demand-gtc.gputechconf.com/gtcnew/on-demand-gtc.php?searchByKeyword=DirectCompute&searchItems=&sessionTopic=&sessionEvent=&sessionYear=&sessionFormat=&submit=&select=+
=======
