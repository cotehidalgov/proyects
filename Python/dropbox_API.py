# coding=utf-8
__author__ = 'MaríaJosé'

import dropbox
import requests
import webbrowser
from PyQt4 import QtGui, QtWebKit
import threading
import os


class Disco(QtGui.QFileDialog):
    def __init__(self):
        super().__init__()


class Visualizador(QtGui.QWidget):
    def __init__(self, dbx):
        super().__init__()
        self.dbx = dbx

        self.boton1 = QtGui.QPushButton("&Subir Archivo", self)
        self.boton1.resize(self.boton1.sizeHint())
        self.boton1.move(5, 5)
        self.boton1.clicked.connect(self.boton1_presionado)

        self.boton2 = QtGui.QPushButton("&Descargar Archivo", self)
        self.boton2.resize(self.boton2.sizeHint())
        self.boton2.move(5, 40)
        self.boton2.clicked.connect(self.boton2_presionado)

        self.boton3 = QtGui.QPushButton("&Crear Carpeta", self)
        self.boton3.resize(self.boton3.sizeHint())
        self.boton3.move(5, 75)
        self.boton3.clicked.connect(self.boton3_presionado)

        self.boton4 = QtGui.QPushButton("&Cambiar Nombre", self)
        self.boton4.resize(self.boton4.sizeHint())
        self.boton4.move(150, 5)
        self.boton4.clicked.connect(self.boton4_presionado)

        self.boton5 = QtGui.QPushButton("&Mover archivo/carpeta a destino", self)
        self.boton5.resize(self.boton5.sizeHint())
        self.boton5.move(150, 40)
        self.boton5.clicked.connect(self.boton5_presionado)

        self.boton6 = QtGui.QPushButton("&Observar Historial", self)
        self.boton6.resize(self.boton6.sizeHint())
        self.boton6.move(150, 75)
        self.boton6.clicked.connect(self.boton6_presionado)

        self.boton7 = QtGui.QPushButton("&Destino", self)
        self.boton7.resize(self.boton7.sizeHint())
        self.boton7.move(300, 75)
        self.boton7.clicked.connect(self.boton7_presionado)

        self.treeview = QtGui.QTreeView(self)
        self.treeview.setHeaderHidden(True)
        self.treeview.setGeometry(5, 100, 590, 495)

        self.model = QtGui.QStandardItemModel(self.treeview)
        self.treeview.setModel(self.model)
        self.treeview.selectionModel().selectionChanged.connect(self.item_seleccionado)

        self.material = {}
        self.lista = []

        self.req_actuales = 0

        self.crear_arbol("", self.model)

        self.item_activo = self.model
        self.destino = None
        self.v = None
        self.v2 = None

    def crear_arbol(self, path, item_padre):
        def funcion():
            for entry in self.dbx.files_list_folder('{}'.format(path)).entries:
                item = QtGui.QStandardItem(entry.name)
                item.setAccessibleDescription(entry.path_lower)
                item_padre.appendRow(item)
                self.material["{}/{}".format(path, entry.name)] = item
                if isinstance(entry, dropbox.files.FolderMetadata):
                    if dbx.files_list_folder("{}".format(entry.path_lower)).entries:
                        self.crear_arbol(entry.path_lower, item)

        thread = threading.Thread(target=funcion)
        thread.start()

    def obtener_item_padre(self):
        for ubicacion in self.material:
            if ubicacion.lower() == self.item_activo.accessibleDescription():
                item_padre = self.material[ubicacion]
                return item_padre

    def item_seleccionado(self):
        idx = self.treeview.selectedIndexes()[0]
        self.item_activo = idx.model().itemFromIndex(idx)

    def boton1_presionado(self):
        direccion = Disco.getOpenFileName()
        lista = os.path.split(direccion)
        if direccion:
            self.dbx.files_upload(os.path.join(lista[0], lista[1]),
                                  "{}/{}".format(self.item_activo.accessibleDescription(), lista[1]))
            item = QtGui.QStandardItem(lista[1])
            item.setAccessibleDescription(self.item_activo.accessibleDescription())
            item_padre = self.obtener_item_padre()
            item_padre.appendRow(item)
            self.material["{}/{}".format(self.item_activo.accessibleDescription(), lista[1])] = item

    def boton2_presionado(self):
        # hacerlo para la raiz tambn
        a, nombre = os.path.split(self.item_activo.accessibleDescription())
        direccion = Disco.getSaveFileName(None, nombre)
        lista = os.path.split(direccion)

        if direccion:
            self.dbx.files_download_to_file(os.path.join(lista[0], lista[1]), self.item_activo.accessibleDescription())

    def boton3_presionado(self):
        self.dbx.files_create_folder("{}/New Directory".format(self.item_activo.accessibleDescription()))
        item = QtGui.QStandardItem("New Directory")
        item.setAccessibleDescription(self.item_activo.accessibleDescription())
        item_padre = self.obtener_item_padre()
        item_padre.appendRow(item)
        self.material["{}/New Directory".format(self.item_activo.accessibleDescription())] = item

    def boton4_presionado(self):
        self.v = Ventana(self)
        self.v.setVisible(True)

    def boton5_presionado(self):
        path, nombre = os.path.split(self.item_activo.accessibleDescription())
        if self.destino is not None:
            self.dbx.files_move("{}/{}".format(path, nombre),
                                "{}/{}".format(self.destino.accessibleDescription(), nombre))
            item_padre = self.obtener_item_padre()
            item_padre.removeRow(self.item_activo.row())
            item = QtGui.QStandardItem(nombre)
            item.setAccessibleDescription("{}/{}".format(self.destino.accessibleDescription(), nombre))
            self.destino.appendRow(item)
            del self.material["{}/{}".format(path, nombre)]
            self.material["{}/{}".format(self.destino.accessibleDescription(), nombre)] = item

    def boton6_presionado(self):
        pass

    def boton7_presionado(self):
        self.destino = self.item_activo

    def obtener_nombre(self):
        new_name = self.v.edit.text()
        self.v.close()
        for ubicacion in self.material:
            if ubicacion.lower() == self.item_activo.accessibleDescription():
                item = self.material[ubicacion]
                item.setText(new_name)
                del self.material[ubicacion]
                path, nombre = os.path.split(self.item_activo.accessibleDescription())
                self.dbx.files_move("{}/{}".format(path, nombre), "{}/{}".format(path, new_name))
                self.material["{}/{}".format(self.item_activo.accessibleDescription(), new_name)] = item
                break


class Ventana(QtGui.QWidget):
    def __init__(self, visualizador):
        super().__init__()
        self.visualizador = visualizador

        self.setGeometry(100, 100, 250, 100)
        self.label = QtGui.QLabel("Ingrese el nombre:", self)
        self.label.move(5, 5)
        self.edit = QtGui.QLineEdit(" ", self)
        self.edit.move(5, 40)
        self.boton = QtGui.QPushButton("&Cambiar Nombre", self)
        self.boton.move(5, 75)
        self.boton.clicked.connect(self.visualizador.obtener_nombre)
        self.show()


class Inicio(QtGui.QWidget):
    def __init__(self, dbx):
        super().__init__()
        self.setGeometry(300, 200, 300, 100)
        self.setWindowTitle("Dropbox")
        visualizador = dbx.users_get_current_account()
        self.nombre = visualizador.name.display_name
        self.label = QtGui.QLabel(
            "Bienvenido {}! Debes seleccionar un item \ny luego apretar la funcion que desees hacer.".format(
                self.nombre), self)
        self.label.move(5, 5)
        self.boton = QtGui.QPushButton("&Aceptar", self)
        self.boton.move(100, 65)
        self.boton.clicked.connect(self.cerrar)
        self.show()

    def cerrar(self):
        self.close()


class MainWindow(QtGui.QMainWindow):
    def __init__(self, dbx):
        super().__init__()

        self.setWindowTitle("Visualizador de archivos")
        self.setFixedSize(600, 600)

        self.dbx = dbx
        self.inicio = Inicio(dbx)
        self.visualizador = Visualizador(dbx)
        self.setCentralWidget(self.visualizador)


if __name__ == "__main__":
    KEY = "key"
    SECRET = "secret"

    url1 = "https://www.dropbox.com/1/oauth2/authorize?response_type=code&client_id={}".format(KEY)

    webbrowser.open_new(url1)

    CODE = input("Ingrese codigo: ")

    url2 = "https://api.dropboxapi.com/1/oauth2/token"
    response2 = requests.post(url2, params={"code": CODE, "grant_type": "authorization_code", "client_id": KEY,
                                            "client_secret": SECRET})
    diccionario = response2.json()

    dbx = dropbox.Dropbox(diccionario["access_token"])
    dbx = dropbox.Dropbox("xco9O4w-WkQAAAAAAAAEFFz3Yntyo_qrTSCkNeeYkg92F_e67rZ8b5FSMJ589w8b")
    app = QtGui.QApplication([])
    main_window = MainWindow(dbx)
    main_window.show()
    app.exec_()
