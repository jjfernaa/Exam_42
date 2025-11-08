# TUTORIAL PASO A PASO: FT_POPEN

## 🎯 OBJETIVO
Implementar `int ft_popen(const char *file, char *const av[], int type)` que lance un ejecutable y retorne un descriptor conectado a su entrada o salida.

---

## 📋 ANÁLISIS DEL PROBLEMA

### **¿Qué hace popen del sistema?**
```c
// popen() estándar permite:
FILE *fp = popen("ls -la", "r");  // Leer la salida del comando
FILE *fp = popen("grep hello", "w"); // Escribir a la entrada del comando
```

### **Nuestro ft_popen**:
```c
int fd = ft_popen("ls", (char *const[]){"ls", "-la", NULL}, 'r');
// fd conectado a la SALIDA de "ls -la" (podemos leer de fd)

int fd = ft_popen("grep", (char *const[]){"grep", "hello", NULL}, 'w');  
// fd conectado a la ENTRADA de "grep hello" (podemos escribir a fd)
```

### **Casos de uso**:
```c
// Tipo 'r': Leer salida del comando
int fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');
char buffer[1024];
read(fd, buffer, 1024);  // Lee la salida de ls

// Tipo 'w': Escribir a la entrada del comando  
int fd = ft_popen("grep", (char *const[]){"grep", "hello", NULL}, 'w');
write(fd, "hello world\n", 12);  // grep procesará esta línea
```

---

## 🧠 COMPRENSIÓN CONCEPTUAL

### **¿Cómo funciona internamente?**

**Tipo 'r' (read)**:
```
Nuestro programa → read(fd) ← pipe ← stdout del comando
                                   ↑
                              (ls ejecutándose)
```

**Tipo 'w' (write)**:
```
Nuestro programa → write(fd) → pipe → stdin del comando
                                        ↓
                                  (grep ejecutándose)
```

### **¿Por qué fork()?**
- El comando debe ejecutarse en **paralelo** con nuestro programa
- Nosotros continuamos ejecutando mientras el comando corre
- La comunicación es a través del pipe

### **¿Qué descriptores retornar?**
- **Tipo 'r'**: Retornamos `pipefd[0]` (read end) para leer la salida del comando
- **Tipo 'w'**: Retornamos `pipefd[1]` (write end) para escribir a la entrada del comando

---

## 🔧 IMPLEMENTACIÓN PASO A PASO

### **PASO 1: Validación de Parámetros**

```c
int ft_popen(const char *file, char *const av[], int type)
{
    // Validar parámetros
    if (!file || !av || (type != 'r' && type != 'w'))
        return -1;
    
    // ... resto del código
}
```

**¿Por qué validar?**
- `file`: Necesitamos el programa a ejecutar
- `av`: Necesitamos los argumentos (mínimo av[0])
- `type`: Solo 'r' o 'w' son válidos

### **PASO 2: Crear el Pipe**

```c
int pipefd[2];
if (pipe(pipefd) == -1)
    return -1;  // Error creando pipe
```

**Recuerda**:
- `pipefd[0]`: Read end (para leer datos)
- `pipefd[1]`: Write end (para escribir datos)

### **PASO 3: Fork del Proceso**

```c
pid_t pid = fork();
if (pid == -1) {
    // Error en fork - limpiar recursos
    close(pipefd[0]);
    close(pipefd[1]); 
    return -1;
}
```

**Gestión de errores**: Si fork falla, debemos cerrar el pipe creado.

### **PASO 4: Configuración del Proceso Hijo**

```c
if (pid == 0) {  // Proceso hijo
    if (type == 'r') {
        // Queremos leer la SALIDA del comando
        // → El comando debe escribir al pipe
        close(pipefd[0]);  // Hijo no lee del pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
            exit(1);
        close(pipefd[1]);  // Ya duplicado, cerrar original
    } 
    else {  // type == 'w'
        // Queremos escribir a la ENTRADA del comando  
        // → El comando debe leer del pipe
        close(pipefd[1]);  // Hijo no escribe al pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1)
            exit(1);
        close(pipefd[0]);  // Ya duplicado, cerrar original
    }
    
    execvp(file, av);
    exit(1);  // Solo se ejecuta si execvp falla
}
```

**Lógica visual para tipo 'r'**:
```
HIJO (comando):
stdout → pipefd[1] → pipe → pipefd[0] → PADRE (nuestro programa)

Ejemplo: ls
stdout del ls → pipe → nosotros leemos la lista de archivos
```

**Lógica visual para tipo 'w'**:
```
PADRE (nuestro programa):
write → pipefd[1] → pipe → pipefd[0] → stdin del HIJO (comando)

Ejemplo: grep hello  
nosotros escribimos texto → pipe → stdin del grep
```

### **PASO 5: Retorno del Proceso Padre**

```c
// Proceso padre
if (type == 'r') {
    // Vamos a leer del pipe
    close(pipefd[1]);    // No escribimos
    return pipefd[0];    // Retornamos descriptor de lectura
} 
else {
    // Vamos a escribir al pipe  
    close(pipefd[0]);    // No leemos
    return pipefd[1];    // Retornamos descriptor de escritura
}
```

**¿Por qué cerrar descriptores?**
- **Tipo 'r'**: Solo necesitamos leer, cerramos el write end
- **Tipo 'w'**: Solo necesitamos escribir, cerramos el read end
- **Importante**: Evita deadlocks y libera recursos

---

## 🔍 CÓDIGO COMPLETO EXPLICADO

```c
int ft_popen(const char *file, char *const av[], int type)
{
    // 1. Validación
    if (!file || !av || (type != 'r' && type != 'w'))
        return -1;

    // 2. Crear pipe
    int pipefd[2];
    if (pipe(pipefd) == -1)
        return -1;

    // 3. Fork
    pid_t pid = fork();
    if (pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }

    if (pid == 0) {  // 4. Proceso hijo
        if (type == 'r') {
            // Comando escribe al pipe
            close(pipefd[0]);
            if (dup2(pipefd[1], STDOUT_FILENO) == -1)
                exit(1);
            close(pipefd[1]);
        } else {
            // Comando lee del pipe
            close(pipefd[1]);  
            if (dup2(pipefd[0], STDIN_FILENO) == -1)
                exit(1);
            close(pipefd[0]);
        }
        
        execvp(file, av);
        exit(1);
    }

    // 5. Proceso padre - retornar descriptor apropiado
    if (type == 'r') {
        close(pipefd[1]);
        return pipefd[0];  // Leer la salida del comando
    } else {
        close(pipefd[0]); 
        return pipefd[1];  // Escribir a la entrada del comando
    }
}
```

---

## 🎨 EJEMPLOS PRÁCTICOS

### **Ejemplo 1: Leer salida de ls**
```c
int main() {
    int fd = ft_popen("ls", (char *const[]){"ls", "-la", NULL}, 'r');
    if (fd == -1) {
        printf("Error\n");
        return 1;
    }
    
    char buffer[1024];
    int bytes = read(fd, buffer, 1023);
    buffer[bytes] = '\0';
    printf("Salida de ls:\n%s", buffer);
    
    close(fd);
    return 0;
}
```

**Flujo**:
1. `ft_popen` ejecuta `ls -la` 
2. La salida de ls va al pipe
3. Nosotros leemos del pipe y mostramos el resultado

### **Ejemplo 2: Escribir a grep**  
```c
int main() {
    int fd = ft_popen("grep", (char *const[]){"grep", "hello", NULL}, 'w');
    if (fd == -1) {
        printf("Error\n");
        return 1;
    }
    
    write(fd, "hello world\n", 12);
    write(fd, "goodbye world\n", 14);
    write(fd, "hello again\n", 12);
    
    close(fd);  // grep terminará y mostrará las líneas con "hello"
    sleep(1);   // Dar tiempo a grep para procesar
    return 0;
}
```

**Flujo**:
1. `ft_popen` ejecuta `grep hello`
2. Escribimos texto al pipe  
3. grep lee del pipe y muestra líneas que contienen "hello"

---

## 🚀 COMPARACIÓN CON PICOSHELL

### **Similitudes**:
- Ambos usan `pipe()`, `fork()`, `dup2()`, `execvp()`
- Ambos conectan procesos mediante pipes
- Ambos manejan descriptores cuidadosamente

### **Diferencias clave**:

| Aspecto | ft_popen | picoshell |
|---------|----------|-----------|
| Procesos | 2 (padre + 1 hijo) | N+1 (padre + N hijos) |
| Pipes | 1 pipe | N-1 pipes |
| Conexión | Padre ↔ Hijo | Hijo₁ → Hijo₂ → Hijo₃ |
| Sincronización | No hay wait() | wait() múltiple |
| Retorno | Descriptor activo | Código de éxito |

### **¿Por qué ft_popen NO hace wait()?**
- El proceso hijo sigue ejecutándose
- El padre puede seguir leyendo/escribiendo
- Es **asíncrono** por diseño

---

## 🐛 ERRORES COMUNES Y CÓMO EVITARLOS

### **❌ Error 1: Confundir los descriptores**
```c
// MAL: Para tipo 'r'
return pipefd[1];  // ¡Incorrecto! Retorna write end

// BIEN: Para tipo 'r'  
return pipefd[0];  // Correcto: retorna read end
```

### **❌ Error 2: No cerrar descriptores**
```c
// MAL: No cerrar en el padre
if (type == 'r') {
    return pipefd[0];  // ¡Descriptor pipefd[1] queda abierto!
}

// BIEN: Cerrar descriptor no usado
if (type == 'r') {
    close(pipefd[1]);  // Cerrar write end
    return pipefd[0];
}
```

### **❌ Error 3: No limpiar en caso de error**
```c
// MAL:
pid_t pid = fork();
if (pid == -1)
    return -1;  // ¡Pipe queda abierto!

// BIEN:
pid_t pid = fork();  
if (pid == -1) {
    close(pipefd[0]);
    close(pipefd[1]);
    return -1;
}
```

### **❌ Error 4: Mal orden en dup2**
```c
// MAL: Cerrar antes de dup2
close(pipefd[1]);
dup2(pipefd[1], STDOUT_FILENO);  // ¡pipefd[1] ya está cerrado!

// BIEN: dup2 primero, luego cerrar  
dup2(pipefd[1], STDOUT_FILENO);
close(pipefd[1]);
```

---

## 🎯 CASOS EXTREMOS PARA TESTEAR

### **Validación de parámetros**:
```c
ft_popen(NULL, argv, 'r');           // → -1
ft_popen("ls", NULL, 'r');           // → -1  
ft_popen("ls", argv, 'x');           // → -1 (tipo inválido)
```

### **Comando inexistente**:
```c
ft_popen("comandoquenoexiste", argv, 'r');  // → descriptor válido
// Pero al leer, no habrá datos (execvp falla en el hijo)
```

### **Un solo comando**:
```c
ft_popen("ls", (char *const[]){"ls", NULL}, 'r');  // → debe funcionar
```

---

## 🧪 EJERCICIO DE AUTOEVALUACIÓN

**Sin mirar el código, responde**:

1. **Para tipo 'w', ¿qué descriptor retornamos?**
   - [ ] pipefd[0] 
   - [ ] pipefd[1]

2. **En el hijo con tipo 'r', ¿qué hacemos con pipefd[0]?**
   - [ ] Lo duplicamos a STDIN
   - [ ] Lo duplicamos a STDOUT  
   - [ ] Lo cerramos

3. **¿Por qué no hacemos wait() en ft_popen?**
   - [ ] Es un error, deberíamos hacer wait()
   - [ ] El proceso hijo sigue ejecutándose después del return
   - [ ] No es necesario porque no hay pipes

4. **¿Qué pasa si execvp() falla en el hijo?**
   - [ ] El padre retorna -1
   - [ ] El hijo hace exit(1)
   - [ ] El programa se bloquea

**Respuestas**: 1-b, 2-c, 3-b, 4-b

---

## 🎯 ¿ESTÁS LISTO?

**Puedes pasar al siguiente ejercicio si**:
- ✅ Entiendes la diferencia entre tipo 'r' y 'w'
- ✅ Sabes qué descriptor retornar en cada caso
- ✅ Entiendes por qué no hacer wait()  
- ✅ Puedes implementar ft_popen desde cero en 15 minutos

**Próximo paso**: sandbox (manejo de señales y timeouts) 🚀