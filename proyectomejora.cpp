#include <iostream>
#include <string>
#include <iomanip>
#include <cstdio> // Para manejo de archivos
#include <xlsxio_read.h>
#include <xlsxio_write.h>
#include <locale.h>
using namespace std;
int setlocale(LC_ALL, "espanish");
struct Producto {
    int codigo;
    char categoria[50];
    double precioPorUnidad;
    int stock;
};

const int MAX_PRODUCTOS = 100;
Producto productos[MAX_PRODUCTOS];
int numProductos = 0;

bool cargarProductosDesdeExcel() {
    xlsxioreader xls;
    const char* xlsPath = "productos.xlsx";
    xls = xlsxioread_open(xlsPath);
    if (!xls) {
        cout << "Error al abrir el archivo Excel para lectura." << endl;
        return false;
    }

    int row = 0;
    while (xlsxioread_next_row(xls)) {
        if (row >= MAX_PRODUCTOS) {
            cout << "Se ha alcanzado el límite máximo de productos." << endl;
            break;
        }

        xlsxioreadersheet sheet = xlsxioread_sheet_open(xls, NULL, XLSXIOREAD_SKIP_EMPTY_ROWS);
        if (sheet == NULL) {
            break;
        }

        int col = 0;
        while (xlsxioread_next_cell(sheet)) {
            const char* value = xlsxioread_cell_value(sheet);
            switch (col) {
                case 0:
                    productos[row].codigo = atoi(value);
                    break;
                case 1:
                    strncpy(productos[row].categoria, value, sizeof(productos[row].categoria) - 1);
                    productos[row].categoria[sizeof(productos[row].categoria) - 1] = '\0';
                    break;
                case 2:
                    productos[row].precioPorUnidad = atof(value);
                    break;
                case 3:
                    productos[row].stock = atoi(value);
                    break;
            }
            col++;
        }
        xlsxioread_sheet_close(sheet);
        row++;
    }

    xlsxioread_close(xls);
    numProductos = row;
    return true;
}

bool guardarProductosEnExcel() {
    xlsxiowriter xls;
    const char* xlsPath = "productos.xlsx";
    xls = xlsxiowrite_open(xlsPath);
    if (!xls) {
        cout << "Error al abrir el archivo Excel para escritura." << endl;
        return false;
    }

    xlsxiowrite_set_sheet_name(xls, "Productos");

    for (int i = 0; i < numProductos; ++i) {
        xlsxiowrite_add_cell_int(xls, productos[i].codigo);
        xlsxiowrite_add_cell_string(xls, productos[i].categoria);
        xlsxiowrite_add_cell_float(xls, productos[i].precioPorUnidad, XLSXIOWRITE_NUMBER_FORMAT_FLOAT);
        xlsxiowrite_add_cell_int(xls, productos[i].stock);
        xlsxiowrite_next_row(xls);
    }

    xlsxiowrite_close(xls);
    return true;
}

void insertar() {
    if (numProductos < MAX_PRODUCTOS) {
        Producto nuevoProducto;

        cout << "Ingrese el código del producto: ";
        cin >> nuevoProducto.codigo;
        cin.ignore(); // Limpiar el buffer de entrada
        
        cout << "Ingrese la categoría del producto: ";
        cin.getline(nuevoProducto.categoria, 50);

        cout << "Ingrese el precio por unidad del producto: ";
        cin >> nuevoProducto.precioPorUnidad;

        cout << "Ingrese el stock del producto: ";
        cin >> nuevoProducto.stock;

        productos[numProductos++] = nuevoProducto;
        guardarProductosEnExcel();
        cout << "Producto ingresado correctamente." << endl;
    } else {
        cout << "No es posible agregar más productos. Límite alcanzado." << endl;
    }
}

void actualizar() {
    if (numProductos == 0) {
        cout << "No hay productos para actualizar." << endl;
        return;
    }

    int codigoActualizar;
    cout << "Ingrese el código del producto a actualizar: ";
    cin >> codigoActualizar;

    bool encontrado = false;
    for (int i = 0; i < numProductos; ++i) {
        if (productos[i].codigo == codigoActualizar) {
            cout << "Actualización de producto:" << endl;
            cout << "Categoría actual: " << productos[i].categoria << endl;
            cout << "Nuevo precio por unidad: ";
            cin >> productos[i].precioPorUnidad;
            cout << "Nuevo stock del producto: ";
            cin >> productos[i].stock;

            guardarProductosEnExcel();
            encontrado = true;
            cout << "Producto actualizado correctamente." << endl;
            break;
        }
    }

    if (!encontrado) {
        cout << "Producto no encontrado." << endl;
    }
}

void eliminar() {
    if (numProductos == 0) {
        cout << "No hay productos para eliminar." << endl;
        return;
    }

    int codigoEliminar;
    cout << "Ingrese el código del producto a eliminar: ";
    cin >> codigoEliminar;

    bool encontrado = false;
    for (int i = 0; i < numProductos; ++i) {
        if (productos[i].codigo == codigoEliminar) {
            encontrado = true;
            for (int j = i; j < numProductos - 1; ++j) {
                productos[j] = productos[j + 1];
            }
            numProductos--;

            guardarProductosEnExcel();
            cout << "Producto eliminado correctamente." << endl;
            break;
        }
    }

    if (!encontrado) {
        cout << "Producto no encontrado." << endl;
    }
}

void mostrarProductos() {
    if (numProductos == 0) {
        cout << "No hay productos para mostrar." << endl;
        return;
    }

    cout << "\nListado de Productos:\n";
    for (int i = 0; i < numProductos; ++i) {
        cout << "Código: " << productos[i].codigo << endl;
        cout << "Categoría: " << productos[i].categoria << endl;
        cout << "Precio por unidad: $" << productos[i].precioPorUnidad << endl;
        cout << "Stock disponible: " << productos[i].stock << " unidades\n";
        cout << "---------------------------\n";
    }
}

void obtenerContrasena(string& contrasena) {
    char caracter;
    contrasena = "";
    while ((caracter = getch()) != 13) { // 13 es el código ASCII para Enter
        if (caracter == 8) { // 8 es el código ASCII para Backspace
            if (!contrasena.empty()) {
                cout << "\b \b"; // Borra el último carácter en la pantalla
                contrasena = contrasena.substr(0, contrasena.length() - 1);
            }
        } else {
            contrasena.push_back(caracter);
            cout << "*";
        }
    }
    cout << endl;
}

int main() {
    string usuario;
    string contrasena;

    cargarProductosDesdeExcel();

    bool salir = false;

    do {
        cout << "+-----------------------------------------------+" << endl;
        cout << "|              INGRESE EL USUARIO               |" << endl;
        cout << "+-----------------------------------------------+" << endl;
        cin >> usuario;
        cin.ignore();

        cout << "+-----------------------------------------------+" << endl;
        cout << "|            INGRESE LA CONTRASEÑA              |" << endl;
        cout << "+-----------------------------------------------+" << endl;
        obtenerContrasena(contrasena);

        if (usuario == "admin" && contrasena == "admin123") {
            do {
                mostrarMenuAdmin();
                int opcion;
                cin >> opcion;
                switch (opcion) {
                    case 1:
                        insertar();
                        break;
                    case 2:
                        buscar();
                        break;
                    case 3:
                        actualizar();
                        break;
                    case 4:
                        eliminar();
                        break;
                    case 5:
                        mostrarProductos();
                        break;
                    case 6:
                        cout << "Saliendo del programa..." << endl;
                        salir = true;
                        break;
                    default:
                        cout << "Opción inválida. Intente de nuevo." << endl;
                }
            } while (!salir);
        } else if (usuario == "empleado" && contrasena == "empleado123") {
            do {
                mostrarMenuEmpleado();
                int opcion;
                cin >> opcion;
                switch (opcion) {
                    case 1:
                        buscar();
                        break;
                    case 2:
                        cout << "Saliendo del programa..." << endl;
                        salir = true;
                        break;
                    default:
                        cout << "Opción inválida. Intente de nuevo." << endl;
                }
            } while (!salir);
        } else {
            cout << "Usuario o contraseña incorrectos. Intente de nuevo." << endl;
        }

        salir = false;

    } while (true);

    return 0;
}

