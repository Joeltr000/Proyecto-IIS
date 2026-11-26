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

/* ========== HU-06: Ejecutar intercambio ========== */
static int asegurar_capacidad_historial(SistemaTrueque *sys) {
    if (sys->num_tx < sys->cap_tx) {
        return 1;
    }
    int nueva_cap = (sys->cap_tx == 0) ? 2 : sys->cap_tx * 2;
    Transaccion *temp = realloc(sys->historial, (size_t)nueva_cap * sizeof(Transaccion));
    if (temp == NULL) {
        printf("Error: No se pudo asignar memoria para el historial.\n");
        return 0;
    }
    sys->historial = temp;
    sys->cap_tx = nueva_cap;
    return 1;
}

void registrar_transaccion(SistemaTrueque *sys, const char *oferente, const char *receptor,
                         const char *detalle, int es_viable) {
    if (!asegurar_capacidad_historial(sys)) {
        return;
    }
    Transaccion *tx = &sys->historial[sys->num_tx];
    tx->id_tx = sys->num_tx + 1;
    snprintf(tx->oferente, sizeof(tx->oferente), "%s", oferente);
    snprintf(tx->receptor, sizeof(tx->receptor), "%s", receptor);
    snprintf(tx->detalle, sizeof(tx->detalle), "%s", detalle);
    tx->es_viable = es_viable;
    sys->num_tx++;
}

int ejecutar_intercambio(SistemaTrueque *sys, int id_oferente, const char *nom_art_ofrecido, int cant_ofrecida,
                       int id_receptor, const char *nom_art_solicitado, int cant_solicitada) {
    if (sys == NULL || cant_ofrecida <= 0 || cant_solicitada <= 0) {
        return 0;
    }

    int idx_of = buscar_por_id(sys, id_oferente);
    int idx_rec = buscar_por_id(sys, id_receptor);
    if (idx_of == -1 || idx_rec == -1 || idx_of == idx_rec) {
        printf("Error: IDs invalidos o iguales.\n");
        return 0;
    }

    Superviviente *of = &sys->usuarios[idx_of];
    Superviviente *rec = &sys->usuarios[idx_rec];

    int pos_of = -1;
    for (int i = 0; i < of->num_articulos; i++) {
        if (strcmp(of->inventario[i].nombre, nom_art_ofrecido) == 0) {
            pos_of = i;
            break;
        }
    }

    int pos_rec = -1;
    for (int i = 0; i < rec->num_articulos; i++) {
        if (strcmp(rec->inventario[i].nombre, nom_art_solicitado) == 0) {
            pos_rec = i;
            break;
        }
    }

    if (pos_of == -1 || pos_rec == -1) {
        printf("Error: Uno de los supervivientes no posee el articulo indicado.\n");
        return 0;
    }

    if (of->inventario[pos_of].cantidad < cant_ofrecida ||
        rec->inventario[pos_rec].cantidad < cant_solicitada) {
        printf("Error: Stock insuficiente para realizar el intercambio.\n");
        return 0;
    }

    int val_ofrecido = cant_ofrecida * of->inventario[pos_of].valor_unitario;
    int val_solicitado = cant_solicitada * rec->inventario[pos_rec].valor_unitario;
    int viable = evaluar_viabilidad(val_ofrecido, val_solicitado);

    char detalle[80];
    snprintf(detalle, sizeof(detalle), "%d %s (val %d) <-> %d %s (val %d)",
             cant_ofrecida, nom_art_ofrecido, val_ofrecido,
             cant_solicitada, nom_art_solicitado, val_solicitado);

    if (!viable) {
        printf("Trueque rechazado: no cumple el margen de equidad (<= 20%%).\n");
        registrar_transaccion(sys, of->nombre, rec->nombre, detalle, 0);
        return 0;
    }

    /* Guardar valores unitarios antes de descontar */
    int val_unit_of = of->inventario[pos_of].valor_unitario;
    int val_unit_rec = rec->inventario[pos_rec].valor_unitario;

    /* Descontar sin dejar cantidades negativas */
    of->inventario[pos_of].cantidad -= cant_ofrecida;
    rec->inventario[pos_rec].cantidad -= cant_solicitada;

    /* Sumar articulos recibidos a cada superviviente */
    if (!agregar_articulo(rec, nom_art_ofrecido, cant_ofrecida, val_unit_of)) {
        of->inventario[pos_of].cantidad += cant_ofrecida;
        rec->inventario[pos_rec].cantidad += cant_solicitada;
        printf("Error al transferir articulo al receptor.\n");
        return 0;
    }

    if (!agregar_articulo(of, nom_art_solicitado, cant_solicitada, val_unit_rec)) {
        printf("Advertencia: problema al transferir articulo al oferente.\n");
    }

    registrar_transaccion(sys, of->nombre, rec->nombre, detalle, 1);
    printf("Trueque ejecutado con exito (ID tx: %d).\n", sys->num_tx);
    return 1;
}

void ejecutar_intercambio_menu(SistemaTrueque *sys) {
    if (sys->num_usuarios < 2) {
        printf("Se necesitan al menos 2 supervivientes registrados.\n");
        return;
    }

    int id_of = leer_entero("ID del oferente: ");
    char art_of[30];
    printf("Articulo que ofrece: ");
    leer_texto(art_of, sizeof(art_of));
    int cant_of = leer_entero("Cantidad ofrecida: ");

    int id_rec = leer_entero("ID del receptor: ");
    char art_rec[30];
    printf("Articulo que solicita: ");
    leer_texto(art_rec, sizeof(art_rec));
    int cant_rec = leer_entero("Cantidad solicitada: ");

    ejecutar_intercambio(sys, id_of, art_of, cant_of, id_rec, art_rec, cant_rec);
}

/* ========== HU-07: Reporte de riqueza y escasez ========== */
void reporte_estadisticas(const SistemaTrueque *sys) {
    const int umbral_escasez = 3;

    printf("\n===== REPORTE DE RIQUEZA Y ESCASEZ =====\n");
    if (sys == NULL || sys->num_usuarios == 0) {
        printf("No hay supervivientes registrados para generar el reporte.\n");
        printf("========================================\n");
        return;
    }

    int mayor_valor = -1;
    int idx_mayor = -1;

    printf("\n-- Valor acumulado por superviviente --\n");
    for (int i = 0; i < sys->num_usuarios; i++) {
        const Superviviente *s = &sys->usuarios[i];
        int valor_total = 0;
        for (int j = 0; j < s->num_articulos; j++) {
            valor_total += s->inventario[j].cantidad * s->inventario[j].valor_unitario;
        }
        printf("  * [%d] %s: %d\n", s->id, s->nombre, valor_total);

        if (idx_mayor == -1 || valor_total > mayor_valor) {
            mayor_valor = valor_total;
            idx_mayor = i;
        }
    }

    if (idx_mayor != -1) {
        printf("\nMayor riqueza: %s (ID %d) con valor acumulado de %d\n",
               sys->usuarios[idx_mayor].nombre,
               sys->usuarios[idx_mayor].id,
               mayor_valor);
    }

    /* Contar articulos distintos para sumar cantidades globales sin duplicar */
    int total_slots = 0;
    for (int i = 0; i < sys->num_usuarios; i++) {
        total_slots += sys->usuarios[i].num_articulos;
    }

    printf("\n-- Articulos en escasez (suma global < %d) --\n", umbral_escasez);
    if (total_slots == 0) {
        printf("  No hay articulos registrados en el sistema.\n");
        printf("========================================\n");
        return;
    }

    Articulo *resumen = malloc((size_t)total_slots * sizeof(Articulo));
    if (resumen == NULL) {
        printf("  Error: No se pudo asignar memoria temporal para el reporte.\n");
        printf("========================================\n");
        return;
    }

    int num_unicos = 0;
    for (int i = 0; i < sys->num_usuarios; i++) {
        const Superviviente *s = &sys->usuarios[i];
        for (int j = 0; j < s->num_articulos; j++) {
            int encontrado = 0;
            for (int k = 0; k < num_unicos; k++) {
                if (strcmp(resumen[k].nombre, s->inventario[j].nombre) == 0) {
                    resumen[k].cantidad += s->inventario[j].cantidad;
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado) {
                snprintf(resumen[num_unicos].nombre, sizeof(resumen[num_unicos].nombre),
                         "%s", s->inventario[j].nombre);
                resumen[num_unicos].cantidad = s->inventario[j].cantidad;
                resumen[num_unicos].valor_unitario = s->inventario[j].valor_unitario;
                num_unicos++;
            }
        }
    }

    int hubo_escasez = 0;
    for (int k = 0; k < num_unicos; k++) {
        if (resumen[k].cantidad < umbral_escasez) {
            printf("  * %s: %d unidad(es) en total\n", resumen[k].nombre, resumen[k].cantidad);
            hubo_escasez = 1;
        }
    }

    if (!hubo_escasez) {
        printf("  Ningun articulo esta por debajo de %d unidades.\n", umbral_escasez);
    }

    free(resumen);
    printf("========================================\n");
}

/* ========== HU-08: Guardar datos en archivos ========== */
int guardar_datos(const SistemaTrueque *sys, const char *archivo_final) {
    if (sys == NULL || archivo_final == NULL) {
        return 0;
    }

    const char *archivo_temp = "trueque_datos.tmp";
    FILE *fp = fopen(archivo_temp, "w");
    if (fp == NULL) {
        printf("Error: No se pudo abrir el archivo temporal '%s' para escritura.\n", archivo_temp);
        return 0;
    }

    int ok = 1;
    if (fprintf(fp, "TRUEQUE_V1 %d %d\n", sys->num_usuarios, sys->num_tx) < 0) {
        ok = 0;
    }

    for (int i = 0; i < sys->num_usuarios && ok; i++) {
        const Superviviente *s = &sys->usuarios[i];
        if (fprintf(fp, "U|%d|%s|%d\n", s->id, s->nombre, s->num_articulos) < 0) {
            ok = 0;
            break;
        }
        for (int j = 0; j < s->num_articulos; j++) {
            const Articulo *a = &s->inventario[j];
            if (fprintf(fp, "A|%s|%d|%d\n", a->nombre, a->cantidad, a->valor_unitario) < 0) {
                ok = 0;
                break;
            }
        }
    }

    for (int k = 0; k < sys->num_tx && ok; k++) {
        const Transaccion *tx = &sys->historial[k];
        if (fprintf(fp, "T|%d|%s|%s|%s|%d\n",
                    tx->id_tx, tx->oferente, tx->receptor, tx->detalle, tx->es_viable) < 0) {
            ok = 0;
            break;
        }
    }

    if (fclose(fp) != 0) {
        ok = 0;
    }

    if (!ok) {
        remove(archivo_temp);
        printf("Error: Fallo al escribir los datos en '%s'.\n", archivo_temp);
        return 0;
    }

    if (rename(archivo_temp, archivo_final) != 0) {
        remove(archivo_temp);
        printf("Error: No se pudo reemplazar '%s' con '%s'.\n", archivo_final, archivo_temp);
        return 0;
    }

    printf("Datos guardados correctamente en '%s'.\n", archivo_final);
    return 1;
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

/* ========== HU-09: Recuperar partida al iniciar ========== */
int cargar_datos(SistemaTrueque *sys, const char *archivo) {
    if (sys == NULL || archivo == NULL) {
        return 0;
    }

    inicializar_sistema(sys);

    FILE *fp = fopen(archivo, "r");
    if (fp == NULL) {
        printf("No se encontro archivo previo '%s'. Iniciando sistema vacio.\n", archivo);
        return 1;
    }

    char magic[20];
    int esp_usuarios = 0;
    int esp_tx = 0;
    if (fscanf(fp, "%19s %d %d\n", magic, &esp_usuarios, &esp_tx) != 3 ||
        strcmp(magic, "TRUEQUE_V1") != 0 ||
        esp_usuarios < 0 || esp_tx < 0) {
        printf("Error: Encabezado o formato invalido en '%s'. Se reinicia el sistema.\n", archivo);
        fclose(fp);
        liberar_sistema(sys);
        inicializar_sistema(sys);
        return 0;
    }

    for (int i = 0; i < esp_usuarios; i++) {
        int id = 0;
        char nombre[30];
        int num_arts = 0;
        if (fscanf(fp, "U|%d|%29[^|]|%d\n", &id, nombre, &num_arts) != 3 ||
            nombre[0] == '\0' || num_arts < 0 || buscar_por_id(sys, id) != -1) {
            printf("Error: Registro de superviviente invalido en '%s'.\n", archivo);
            fclose(fp);
            liberar_sistema(sys);
            inicializar_sistema(sys);
            return 0;
        }

        if (sys->num_usuarios == sys->cap_usuarios) {
            int nueva_cap = (sys->cap_usuarios == 0) ? 2 : sys->cap_usuarios * 2;
            Superviviente *temp = realloc(sys->usuarios, (size_t)nueva_cap * sizeof(Superviviente));
            if (temp == NULL) {
                printf("Error: Sin memoria al cargar supervivientes.\n");
                fclose(fp);
                liberar_sistema(sys);
                inicializar_sistema(sys);
                return 0;
            }
            sys->usuarios = temp;
            sys->cap_usuarios = nueva_cap;
        }

        Superviviente *u = &sys->usuarios[sys->num_usuarios];
        u->id = id;
        snprintf(u->nombre, sizeof(u->nombre), "%s", nombre);
        u->inventario = NULL;
        u->num_articulos = 0;
        u->capacidad_inv = 0;
        sys->num_usuarios++;

        for (int j = 0; j < num_arts; j++) {
            char nom_art[30];
            int cant = 0;
            int val = 0;
            if (fscanf(fp, "A|%29[^|]|%d|%d\n", nom_art, &cant, &val) != 3 ||
                nom_art[0] == '\0' || cant <= 0 || val <= 0) {
                printf("Error: Articulo invalido en inventario de '%s'.\n", u->nombre);
                fclose(fp);
                liberar_sistema(sys);
                inicializar_sistema(sys);
                return 0;
            }
            if (!agregar_articulo(u, nom_art, cant, val)) {
                printf("Error: Sin memoria al cargar articulos de '%s'.\n", u->nombre);
                fclose(fp);
                liberar_sistema(sys);
                inicializar_sistema(sys);
                return 0;
            }
        }
    }

    for (int k = 0; k < esp_tx; k++) {
        int id_tx = 0;
        char oferente[30];
        char receptor[30];
        char detalle[80];
        int viable = 0;
        if (fscanf(fp, "T|%d|%29[^|]|%29[^|]|%79[^|]|%d\n",
                   &id_tx, oferente, receptor, detalle, &viable) != 5 ||
            oferente[0] == '\0' || receptor[0] == '\0' || (viable != 0 && viable != 1)) {
            printf("Error: Transaccion invalida en '%s'.\n", archivo);
            fclose(fp);
            liberar_sistema(sys);
            inicializar_sistema(sys);
            return 0;
        }

        if (!asegurar_capacidad_historial(sys)) {
            fclose(fp);
            liberar_sistema(sys);
            inicializar_sistema(sys);
            return 0;
        }

        Transaccion *tx = &sys->historial[sys->num_tx];
        tx->id_tx = id_tx;
        snprintf(tx->oferente, sizeof(tx->oferente), "%s", oferente);
        snprintf(tx->receptor, sizeof(tx->receptor), "%s", receptor);
        snprintf(tx->detalle, sizeof(tx->detalle), "%s", detalle);
        tx->es_viable = viable;
        sys->num_tx++;
    }

    fclose(fp);
    printf("Partida cargada desde '%s' (%d supervivientes, %d transacciones).\n",
           archivo, sys->num_usuarios, sys->num_tx);
    return 1;
}

/* ========== HU-10: Eliminar registro y consistencia ========== */
int eliminar_superviviente(SistemaTrueque *sys, int id) {
    if (sys == NULL || sys->num_usuarios == 0) {
        printf("No hay supervivientes registrados para eliminar.\n");
        return 0;
    }

    int idx = buscar_por_id(sys, id);
    if (idx == -1) {
        printf("No existe un superviviente con ID %d.\n", id);
        return 0;
    }

    char nombre_eliminado[30];
    snprintf(nombre_eliminado, sizeof(nombre_eliminado), "%s", sys->usuarios[idx].nombre);

    /* Liberar primero el inventario interno del superviviente eliminado */
    free(sys->usuarios[idx].inventario);
    sys->usuarios[idx].inventario = NULL;
    sys->usuarios[idx].num_articulos = 0;
    sys->usuarios[idx].capacidad_inv = 0;

    /* Recorrer elementos posteriores para cerrar el hueco */
    for (int i = idx; i < sys->num_usuarios - 1; i++) {
        sys->usuarios[i] = sys->usuarios[i + 1];
    }

    sys->num_usuarios--;

    /* Si no quedan usuarios, liberar arreglo principal */
    if (sys->num_usuarios == 0) {
        free(sys->usuarios);
        sys->usuarios = NULL;
        sys->cap_usuarios = 0;
    }

    printf("Superviviente '%s' (ID %d) eliminado sin dejar huecos.\n", nombre_eliminado, id);
    return 1;
}

void eliminar_superviviente_menu(SistemaTrueque *sys) {
    if (sys->num_usuarios == 0) {
        printf("No hay supervivientes registrados.\n");
        return;
    }
    int id = leer_entero("ID del superviviente a eliminar: ");
    eliminar_superviviente(sys, id);
}

/* ========== HU-11: Cierre sin fugas y defensa ========== */
void liberar_todo(SistemaTrueque *sys) {
    liberar_sistema(sys);
    printf("Memoria del sistema liberada y punteros reiniciados a NULL.\n");
}

void verificar_estado_cierre(const SistemaTrueque *sys) {
    printf("\n===== VERIFICACION DE CIERRE (HU-11) =====\n");
    if (sys == NULL) {
        printf("Puntero de sistema nulo.\n");
        return;
    }
    printf("Usuarios activos: %d | Capacidad: %d | Puntero usuarios: %s\n",
           sys->num_usuarios, sys->cap_usuarios,
           (sys->usuarios == NULL) ? "NULL" : "ACTIVO");
    printf("Transacciones: %d | Capacidad: %d | Puntero historial: %s\n",
           sys->num_tx, sys->cap_tx,
           (sys->historial == NULL) ? "NULL" : "ACTIVO");
    printf("==========================================\n");
}

/* ========== HU-00: main ========== */
int main(void) {
    SistemaTrueque sys;
    cargar_datos(&sys, "trueque_datos.txt");

    int opcion;
    do {
        printf("\n--- MENU TRUEQUE ---\n");
        printf("1. Registrar superviviente (HU-02)\n");
        printf("2. Agregar articulo (HU-03)\n");
        printf("3. Mostrar sistema (HU-04)\n");
        printf("4. Evaluar viabilidad (HU-05)\n");
	printf("5. Ejecutar intercambio (HU-06)\n");
	printf("6. Reporte de riqueza y escasez (HU-07)\n");
	printf("7. Guardar datos en archivo (HU-08)\n");
	printf("8. Eliminar superviviente (HU-10)\n");
	printf("9. Verificar estado de memoria/cierre (HU-11)\n");
        printf("0. Salir\n");
        opcion = leer_entero("Opcion: ");

        switch (opcion) {
            case 1: registrar_superviviente(&sys); break;
            case 2: agregar_articulo_menu(&sys); break;
            case 3: mostrar_sistema(&sys); break;
            case 4: probar_viabilidad_menu(); break;
	    case 5: ejecutar_intercambio_menu(&sys); break;
	    case 6: reporte_estadisticas(&sys);break;
	    case 7: guardar_datos(&sys, "trueque_datos.txt"); break;
	    case 8: eliminar_superviviente_menu(&sys); break;
	    case 9: verificar_estado_cierre(&sys); break;
            case 0:
                guardar_datos(&sys, "trueque_datos.txt"); break;
                printf("Saliendo...\n"); break;
            default: printf("Opcion invalida.\n"); break;
        }
    } while (opcion != 0);

    liberar_sistema(&sys);
    verificar_estado_cierre(&sys);
    return 0;
}

