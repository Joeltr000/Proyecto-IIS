#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== HU-01: Estructuras base ========== */

typedef struct {
    char nombre[30];
    int cantidad;
    int valor_unitario;
} Articulo;

typedef struct {
    int id;
    char nombre[30];
    Articulo *inventario;
    int num_articulos;
    int capacidad_inv;
} Superviviente;

typedef struct {
    int id_tx;
    char oferente[30];
    char receptor[30];
    char detalle[80];
    int es_viable;
} Transaccion;

typedef struct {
    Superviviente *usuarios;
    int num_usuarios;
    int cap_usuarios;
    Transaccion *historial;
    int num_tx;
    int cap_tx;
} SistemaTrueque;

/* ========== HU-01: Inicializar ========== */
void inicializar_sistema(SistemaTrueque *sys) {
    sys->usuarios = NULL;
    sys->num_usuarios = 0;
    sys->cap_usuarios = 0;
    sys->historial = NULL;
    sys->num_tx = 0;
    sys->cap_tx = 0;
}

/* ========== Utilidades ========== */
void limpiar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void leer_texto(char *dest, int tam) {
    if (fgets(dest, tam, stdin) != NULL) {
        size_t len = strlen(dest);
        if (len > 0 && dest[len - 1] == '\n') {
            dest[len - 1] = '\0';
        } else {
            limpiar_buffer();
        }
    }
}

int leer_entero(const char *mensaje) {
    int valor;
    printf("%s", mensaje);
    while (scanf("%d", &valor) != 1) {
        printf("Entrada invalida. Intenta de nuevo: ");
        limpiar_buffer();
    }
    limpiar_buffer();
    return valor;
}

/* Busca superviviente por ID, regresa indice o -1 */
int buscar_por_id(const SistemaTrueque *sys, int id) {
    for (int i = 0; i < sys->num_usuarios; i++) {
        if (sys->usuarios[i].id == id) {
            return i;
        }
    }
    return -1;
}

/* ========== HU-02: Registrar superviviente ========== */
/* Sin limite, sin ID repetido, sin nombre repetido, realloc con temporal */
int registrar_superviviente(SistemaTrueque *sys) {
    int id = leer_entero("ID del superviviente: ");

    if (buscar_por_id(sys, id) != -1) {
        printf("Error: Ya existe un superviviente con ID %d\n", id);
        return 0;
    }

    char nombre[30];
    printf("Nombre: ");
    leer_texto(nombre, sizeof(nombre));

    for (int i = 0; i < sys->num_usuarios; i++) {
        if (strcmp(sys->usuarios[i].nombre, nombre) == 0) {
            printf("Error: Ya existe un superviviente con nombre '%s'\n", nombre);
            return 0;
        }
    }

    /* Crecer arreglo si hace falta */
    if (sys->num_usuarios == sys->cap_usuarios) {
        int nueva_cap = (sys->cap_usuarios == 0) ? 2 : sys->cap_usuarios * 2;
        Superviviente *temp = realloc(sys->usuarios, nueva_cap * sizeof(Superviviente));
        if (temp == NULL) {
            printf("Error: No se pudo asignar memoria\n");
            return 0;
        }
        sys->usuarios = temp;
        sys->cap_usuarios = nueva_cap;
    }

    Superviviente *nuevo = &sys->usuarios[sys->num_usuarios];
    nuevo->id = id;
    strcpy(nuevo->nombre, nombre);
    nuevo->inventario = NULL;
    nuevo->num_articulos = 0;
    nuevo->capacidad_inv = 0;

    sys->num_usuarios++;
    printf("Superviviente '%s' registrado con exito.\n", nombre);
    return 1;
}

/* ========== HU-03: Agregar articulo ========== */
/* Inventario dinamico, acumula si ya existe el articulo */
int agregar_articulo(Superviviente *s, const char *nombre_art, int cantidad, int valor_unitario) {
    if (s == NULL || cantidad <= 0 || valor_unitario <= 0) {
        return 0;
    }

    /* Si ya existe, acumula cantidad */
    for (int i = 0; i < s->num_articulos; i++) {
        if (strcmp(s->inventario[i].nombre, nombre_art) == 0) {
            s->inventario[i].cantidad += cantidad;
            /* Actualiza valor por si cambio */
            s->inventario[i].valor_unitario = valor_unitario;
            return 1;
        }
    }

    /* Si no existe, crece inventario */
    if (s->num_articulos == s->capacidad_inv) {
        int nueva_cap = (s->capacidad_inv == 0) ? 2 : s->capacidad_inv * 2;
        Articulo *temp = realloc(s->inventario, nueva_cap * sizeof(Articulo));
        if (temp == NULL) {
            return 0;
        }
        s->inventario = temp;
        s->capacidad_inv = nueva_cap;
    }

    Articulo *nuevo = &s->inventario[s->num_articulos];
    strcpy(nuevo->nombre, nombre_art);
    nuevo->cantidad = cantidad;
    nuevo->valor_unitario = valor_unitario;
    s->num_articulos++;

    return 1;
}

/* Wrapper para pedir datos por teclado - HU-03 */
void agregar_articulo_menu(SistemaTrueque *sys) {
    if (sys->num_usuarios == 0) {
        printf("No hay supervivientes registrados.\n");
        return;
    }
    int id = leer_entero("ID del superviviente: ");
    int idx = buscar_por_id(sys, id);
    if (idx == -1) {
        printf("No existe superviviente con ID %d\n", id);
        return;
    }

    char nombre[30];
    printf("Nombre del articulo (ej: Pilas, Sardinas, Calcetines): ");
    leer_texto(nombre, sizeof(nombre));

    int cantidad = leer_entero("Cantidad: ");
    int valor = leer_entero("Valor unitario: ");

    if (agregar_articulo(&sys->usuarios[idx], nombre, cantidad, valor)) {
        printf("Articulo agregado a %s.\n", sys->usuarios[idx].nombre);
    } else {
        printf("Error al agregar articulo.\n");
    }
}

/* ========== HU-04: Mostrar sistema ========== */
void mostrar_sistema(const SistemaTrueque *sys) {
    printf("\n===== SISTEMA DE TRUEQUE =====\n");
    printf("Total supervivientes: %d\n", sys->num_usuarios);
    printf("Total transacciones: %d\n", sys->num_tx);

    if (sys->num_usuarios == 0) {
        printf("Sin supervivientes registrados.\n");
        return;
    }

    for (int i = 0; i < sys->num_usuarios; i++) {
        const Superviviente *s = &sys->usuarios[i];
        printf("\n--- [%d] %s ---\n", s->id, s->nombre);
        if (s->num_articulos == 0) {
            printf("  Sin articulos.\n");
        } else {
            for (int j = 0; j < s->num_articulos; j++) {
                printf("  * %s | Cant: %d | Valor: %d | Total: %d\n",
                       s->inventario[j].nombre,
                       s->inventario[j].cantidad,
                       s->inventario[j].valor_unitario,
                       s->inventario[j].cantidad * s->inventario[j].valor_unitario);
            }
        }
    }
    printf("==============================\n");
}

/* ========== HU-05: Algoritmo de viabilidad ========== */
/*
  Viable si |ofrecido - solicitado| / solicitado <= 0.20 (20%)
  Regresa 1 si es viable, 0 si no.
*/
int evaluar_viabilidad(int valor_ofrecido, int valor_solicitado) {
    if (valor_solicitado <= 0 || valor_ofrecido < 0) {
        return 0;
    }
    int diferencia = valor_ofrecido - valor_solicitado;
    if (diferencia < 0) {
        diferencia = -diferencia;
    }
    /* Evita flotantes: diferencia * 100 <= solicitado * 20 */
    return (diferencia * 100 <= valor_solicitado * 20);
}

void probar_viabilidad_menu(void) {
    int ofrecido = leer_entero("Valor total ofrecido: ");
    int solicitado = leer_entero("Valor total solicitado: ");

    int viable = evaluar_viabilidad(ofrecido, solicitado);
    int dif = ofrecido - solicitado;
    if (dif < 0) dif = -dif;

    printf("Diferencia: %d\n", dif);
    if (viable) {
        printf("Resultado: VIABLE - Se puede realizar el trueque.\n");
    } else {
        printf("Resultado: NO VIABLE - Diferencia mayor al 20%%.\n");
    }
}

/* ========== Liberar memoria ========== */
void liberar_sistema(SistemaTrueque *sys) {
    for (int i = 0; i < sys->num_usuarios; i++) {
        free(sys->usuarios[i].inventario);
        sys->usuarios[i].inventario = NULL;
    }
    free(sys->usuarios);
    sys->usuarios = NULL;
    free(sys->historial);
    sys->historial = NULL;
    sys->num_usuarios = 0;
    sys->num_tx = 0;
    sys->cap_usuarios = 0;
    sys->cap_tx = 0;
}

/* ========== HU-00: main ========== */
int main(void) {
    SistemaTrueque sys;
    inicializar_sistema(&sys);

    int opcion;
    do {
        printf("\n--- MENU TRUEQUE ---\n");
        printf("1. Registrar superviviente (HU-02)\n");
        printf("2. Agregar articulo (HU-03)\n");
        printf("3. Mostrar sistema (HU-04)\n");
        printf("4. Evaluar viabilidad (HU-05)\n");
        printf("0. Salir\n");
        opcion = leer_entero("Opcion: ");

        switch (opcion) {
            case 1: registrar_superviviente(&sys); break;
            case 2: agregar_articulo_menu(&sys); break;
            case 3: mostrar_sistema(&sys); break;
            case 4: probar_viabilidad_menu(); break;
            case 0: printf("Saliendo...\n"); break;
            default: printf("Opcion invalida.\n"); break;
        }
    } while (opcion != 0);

    liberar_sistema(&sys);
    return 0;
}
