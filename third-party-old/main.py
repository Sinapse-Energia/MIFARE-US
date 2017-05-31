import psycopg2
import sys
import httplib2
import xml.etree.cElementTree as ET
import datetime
import time
from threading import Thread
import os
import concurrent.futures
import subprocess
from xml.dom.minidom import parse, parseString
import os.path
import time
from urllib.parse import urlencode

continue_reading = 1
#def create_getlist(serverip,fechahora,edificio,tipoaula,aula):
  #root = ET.Element("horfeus")
  #doc = ET.SubElement(root, "firma")

  #field1 = ET.SubElement(doc, "ip")
  #field1.text = serverip
  #field2 = ET.SubElement(doc, "TipoCliente")
  #field2.text = "4"
  #field3 = ET.SubElement(doc, "tiempo")
  #field3.text = fechahora
  #field4 = ET.SubElement(doc, "edificio")
  #field4.text = edificio
  #field5 = ET.SubElement(doc, "TipoAula")
  #field5.text = tipoaula
  #field6 = ET.SubElement(doc, "aula")
  #field6.text = aula

  #tree = ET.ElementTree(root)
  #tree.write("getlist.xml")
  #getlist = ET.tostring(root)
  #return getlist

def create_getlist(myip):
  root = ET.Element("horfeus")
  doc = ET.SubElement(root, "aulas")

  field1 = ET.SubElement(doc, "IP")
  field1.text = myip

  tree = ET.ElementTree(root)
  tree.write("getlist.xml")
  getlist = ET.tostring(root)
  return getlist

def create_registro(myip,fechahora,edificio,tipoaula,aula,serie):
  root = ET.Element("horfeus")
  doc = ET.SubElement(root, "firma")
 
  field2 = ET.SubElement(doc, "IP")
  field2.text = myip
  field3 = ET.SubElement(doc, "TipoCliente")
  field3.text = '4'
  field4 = ET.SubElement(doc, "tiempo")
  field4.text = fechahora
  field5 = ET.SubElement(doc, "edificio")
  field5.text = edificio
  field6 = ET.SubElement(doc, "TipoAula")
  field6.text = tipoaula
  field7 = ET.SubElement(doc, "aula")
  field7.text = aula
  field8 = ET.SubElement(doc, "serie")
  field8.text = serie
  tree = ET.ElementTree(root)
  tree.write("registro.xml")
  registro = ET.tostring(root) 

  return registro


def guardaclock():
    (err, out) = subprocess.getstatusoutput('date +"%Y"')
    print(out)
    if(out != '1970'):
        (err, out) = subprocess.getstatusoutput('date +"%Y-%m-%d %H:%M" > clock.log' )
        print(out)

def restauraclock():
    (err, out) = subprocess.getstatusoutput('date +"%Y"')
    print(out)
    if(out == '1970'):
       count = 1 
       data_file=open('clock.log','r')
       for line in data_file:
          print("LEO EL FICHERO DE CLOCK")
          print(line[0:4])
          count+=1
       
       (err,out) = subprocess.getstatusoutput("sudo date --set '" + line + "'")    
       print(err)
       print(out)

def arranca_rails(myip):
    #command = "sudo ntpd -qg"
    #os.system(command)
    #command = "sudo ntpd"
    #os.system(command)
    print("Vemos si existe el fichero")
    if(os.path.isfile("./tmp/pids/server.pid")):
        os.remove("./tmp/pids/server.pid")
    command = "sudo sh execrails.sh " + myip
    #command = "cd /home/pi/Desktop/RFID-US; rails s -b " + myip + " -p 3000 > /home/pi/Desktop/rails_server.log 2>&1 &"
    #print(command)
    os.system(command)

def reiniciaethernet(myip):
    #command = "sudo ifconfig eth0 down"
    #os.system(command)
    #command = "sudo ifconfig eth0 up"
    #os.system(command)	
    #time.sleep(12)
    command = "sudo ifconfig eth0 " + myip
    os.system(command)   	
   
def main():
    restauraclock()
    con = psycopg2.connect(host='localhost', database='BD_RFID', user='sinapse', password='napse1si')
    cur = con.cursor()
    query = "SELECT ipdevice FROM a_serverconfigurations"
    cur.execute(query)
    resultado = cur.fetchone()
    myip=str(resultado[0])
    cur.close()
    print("Arrancamos Rails")
   # if(os.path.isfile("salida.txt")):
   #    os.remove("salida.txt")

    #t1 = Thread(target=arranca_rails, args=(myip,))
    #t1.start()

	
    with concurrent.futures.ThreadPoolExecutor(max_workers=2) as executor:
        future1 = executor.submit( arranca_rails, myip)
        future2 = executor.submit( reading_card)
        future3 = executor.submit( get_list)
        #print( future1.result() )
        #print( future2.result() )
        #print( future3.result() )

	# Welcome message
######################  MOSTRAR POR PANTALLA FECHA-HORA 
    print("Welcome to RFID Sinapse")
    #bucle donde me quedo leyendo continuamente
	# This loop keeps checking for chips. If one is near it will get the UID and authenticate

def reading_card():
    print("Leyendo la tarjeta")
    while continue_reading:
        try:
            print("Leyendo la tarjeta")
           # Intento leer, cuando consiga leer me devolvera el valor correcto
            (err, out) = subprocess.getstatusoutput('/home/pi/Desktop/libnfc-1.7.1/utils/quick_start_example2 r a mycard2.mfd')
            print(err)
            print("output es %s", out)       
            if err == 0:
                open("oklector.log", 'w')
                print("Leido el valor ", out)
                print(out.replace(".",""))
                ########Cuando leo un dato en la tarjeta, envio el registro
		        #en serie almaceno el valor leido siempre que sea valido
                serie = out.replace(".","")
                serie = serie.strip()
                if len(serie) <= 8 and serie.isdigit():    
                   con = psycopg2.connect(host='localhost', database='BD_RFID', user='sinapse', password='napse1si')  
                   cur = con.cursor()
                   query = "SELECT url_server,ipdevice FROM a_serverconfigurations"	      
                   cur.execute(query)
                   resultado = cur.fetchone()
                   a_url=str(resultado[0])
                   myip=str(resultado[1])
                   query = "SELECT d_edificio,d_aula,d_tipoaula,d_codigo FROM d_aulas INNER JOIN d_auladispositivo ON da_idaula=d_id INNER JOIN a_deviceconfiguration ON ad_id=da_iddispositivo"
                   cur.execute(query)
                   resultado = cur.fetchone()	  
                   edificio=str(resultado[0])
                   #print("Edificio: ", edificio)
                   aula=str(resultado[3])
                   tipoaula=str(resultado[2])		
                   cur.close()
                   #print("Cierro el cursor")
                   registro = create_registro(myip,datetime.datetime.now().strftime("%d/%m/%Y %H:%M:%S"),edificio,tipoaula,aula,serie) 
                   #Y aqui tendre que enviar al servidor mi registro de entrada, ademas de mostrar por pantalla que se esta registrando en el servidor
                   con2 = psycopg2.connect(host='localhost', database='BD_RFID', user='sinapse', password='napse1si')
                   cur2 = con2.cursor()
                   query = "SELECT url_server,ipdevice FROM a_serverconfigurations"
                   cur2.execute(query)
                   resultado = cur2.fetchone()
                   a_url=str(resultado[0])
                   myip=str(resultado[1])
                   cur2.close()
                   #solicitudregistro =  "%s%s%s" % ("'",registro.decode(encoding='UTF-8'),"'")
                   solicitudregistro = registro.decode(encoding='UTF-8')
                   print(solicitudregistro)
                   print(a_url)
                   #params = {'solicitud': solicitudregistro}
                   reiniciaethernet(myip)  
                   time.sleep(1)				   
                   for x in range(0, 4):
                      try:    
                         print("Registrando en el servidor")
                         h2 = httplib2.Http(timeout=30, disable_ssl_certificate_validation=True)
                         httplib2.force_exception_to_status_code = True
                         params = {'solicitud': solicitudregistro}
                         (resp_headers, content) = h2.request(a_url, "POST", urlencode(params), headers={'content-type':'application/x-www-form-urlencoded'})
                      except:
                         #Habre tenido un error de conexion con el servidor, timeout, o unreachable
                         print("Error registrando al usuario:", sys.exc_info()[0])
                         f=open("errorconexionregistro.log", 'w')
                         f.write("%s%s%s" % ("Error de conexion con el servidor: ", sys.exc_info()[0]))
                         f.close()
                         continue					 
                      try: 
                         print("Contenido es %s", content)					  
                         dom4 = parseString(content)
					     ##Copio a partir de aqui
                         try:
                            error = dom4.getElementsByTagName("error")[0].firstChild.data
                            #El servidor me ha respondido con algun error
                            print("Error recibido")
                            f = open("errorrecibido.log", 'w')			
                            f.write(error)
                            f.close()
                            break
                         except:
                            try:
                               #El servidor me ha respondido ok
                               registro = dom4.getElementsByTagName("registro")[0].firstChild.data
                               tiempo = dom4.getElementsByTagName("tiempoServidor")[0].firstChild.data
                               documento = dom4.getElementsByTagName("documento")[0].firstChild.data
                               nombre = dom4.getElementsByTagName("nombre")[0].firstChild.data
                            except:
                               f = open("errordom2.log", 'w')			
                               f.write("%s%s" % ("ERROR: ", sys.exc_info()[0]))
                               f.close()
                               break							   
                      except:
                         print("Error registrando al usuario:", sys.exc_info()[0])
                         f = open("errorconexionregistro.log", 'w')			
                         f.write("%s%s%s" % ("Error de conexion con el servidor: ", sys.exc_info()[0]))
                         f.close()
                         continue						 
                      try: 					 
                         con3 = psycopg2.connect(host='localhost', database='BD_RFID', user='sinapse', password='napse1si')
                         #Y ahora aqui actualizo la base de datos
                         cur3 = con3.cursor()
                         cur3.execute("DELETE FROM l_logregistros")
                         cur3.execute("INSERT INTO l_logregistros(l_registro,l_documento,l_nombre,l_datetime) VALUES (%s,%s,%s,%s)",(registro,documento,nombre,tiempo))
                         con3.commit()
                         print("Y ahora creo el fichero de usuario registrado")
                         f= open("usuarioregistrado.log", 'w')
                         f.write(nombre)
                         cur3.close()
                         con3.close()
                         #Y le digo a la aplicacion de escritorio que he registrado correctamente
                         f.close()
                         break
					 				 
                      except:
                         print("Error registrando al usuario:", sys.exc_info()[0])
                         #f = open("errorbbdd.log", 'w')
                         #f.write("%s%s%s%s" % ("ERROR BBDD: ", sys.exc_info()[0], registro, documento, nombre))
                         #f.close()	
                         continue					   
                else:
                   open("reintentarlector.log",'w')
	
            else:
              #####IMPRIMIR AQUI QUE HAY UN ERROR CON EL LECTOR
              open("errorlector.log", 'w') 
        
        except:
            #####IMPRIMIR AQUI QUE HAY UN ERROR CON EL LECTOR
            open("errorlector.log", 'w')
			  
def get_list():
  print("FICHERO DE CONFIGURACION")
  while True:
      try:
        con = psycopg2.connect(host='localhost', database='BD_RFID', user='sinapse', password='napse1si')
        cur = con.cursor()
        query = "SELECT url_server,ipdevice FROM a_serverconfigurations" 
        cur.execute(query)
        resultado = cur.fetchone()
        a_url=str(resultado[0])
        myip=str(resultado[1])
        query = "SELECT d_edificio,d_aula,d_tipoaula FROM d_aulas INNER JOIN d_auladispositivo ON da_idaula=d_id INNER JOIN a_deviceconfiguration ON ad_id=da_iddispositivo"
        cur.execute(query)
        resultado = cur.fetchone()
        edificio=str(resultado[0])
        aula=str(resultado[1])
        tipoaula=str(resultado[2])    
        query = "SELECT refreshtime,refreshtime_error FROM a_serverconfigurations LIMIT 1"
        cur.execute(query)
        resultado = cur.fetchone()
        refreshtime=str(resultado[0])
        refreshtime_error=str(resultado[1])
        cur.close()
        print("Llamo a la funcion getlist")
        #getlist = create_getlist(myip,datetime.datetime.now().strftime("%d/%m/%Y %H:%M:%S"))
        getlist = create_getlist(myip)
        sol = "?solicitud="
        solicitud = "%s%s%s" % (a_url,sol,getlist.decode(encoding='UTF-8'))
        print(solicitud)
        time.sleep(1)
        reiniciaethernet(myip)
        time.sleep(1)
        #h = httplib2.Http(timeout=10, disable_ssl_certificate_validation=True)
        h = httplib2.Http(timeout=20, disable_ssl_certificate_validation=True)
        httplib2.force_exception_to_status_code = True
        try:   
            guardaclock() 
    #################### IMPRIMIR EN PANTALLA ACTUALIZACION         
            print("Actualizacion de configuracion a la url ", a_url)
            (resp_headers, content) = h.request(solicitud, "GET", headers={'content-type':'text/plain'})	
           
            print(content)
            dom3 = parseString(content)
            dispositivo = dom3.getElementsByTagName("dispositivo")[0]
            print(dispositivo.firstChild.data)
 
            aulas = dom3.getElementsByTagName("aula")
            i = 0
            if not (aulas):
                print("No se tiene aula")
                open("erroraulas.log", 'w')
                time.sleep(float(refreshtime_error) * 60)
            for aula in aulas:
                #Borro el fichero de error de conexion porque he tenido respuesta
                open("okconexion.log", 'w') 
                i = i + 1 
                codigo = aula.getElementsByTagName("codigo")[0]
                edificio = aula.getElementsByTagName("edificio")[0]
                tipo = aula.getElementsByTagName("tipo")[0]
                nombre = aula.getElementsByTagName("nombre")[0]
                print("codigo:%s, edificio:%s, tipo:%s, nombre:%s",(codigo.firstChild.data, edificio.firstChild.data, tipo.firstChild.data, nombre.firstChild.data)) 
                #Y ahora aqui actualizo la base de datos
                cur = con.cursor()
                print(dispositivo.firstChild.data)
                cur.execute("UPDATE a_deviceconfiguration SET ad_numserie = %s",(dispositivo.firstChild.data,))
                cur.execute("UPDATE d_aulas SET d_tipoaula = %s, d_aula = %s, d_edificio=%s, d_codigo=%s",(tipo.firstChild.data,nombre.firstChild.data,edificio.firstChild.data,codigo.firstChild.data))
                con.commit()
                cur.close()
                print("Y actualizado todo bien")		
                if(i>1):
                 #################### IMPRIMIR EN PANTALLA ERROR DE MAS DE UNA AULA
                    print("Error, se ha obtenido mas de una aula") 
                    open("erroraulas.log", 'w')
                    time.sleep(float(refreshtime_error) * 60)
                else:
              #################### PROCESAR LA RESPUESTA DEL SERVIDOR PARA ACTUALIZAR MIS DATOS Y MOSTRAR EN PANTALLA
                  print("Getlist es %s", getlist)
                  time.sleep(float(refreshtime) * 60)
              			
        except:
        #################### IMPRIMIR EN PANTALLA ERROR DE CONEXION CON EL SERVIDOR, SEGURAMENTE SEA TIMEOUT
            print("Error peticion getlist:", sys.exc_info()[0])
            open("errorconexion.log", 'w')
            time.sleep(float(refreshtime_error) * 60)
            con = psycopg2.connect(host='localhost', database='BD_RFID', user='sinapse', password='napse1si')
            cur = con.cursor()
            query = "SELECT url_server,ipdevice FROM a_serverconfigurations" 
            cur.execute(query)
            resultado = cur.fetchone()
            a_url=str(resultado[0])
            myip=str(resultado[1])
            reiniciaethernet(myip)      
            time.sleep(1)
      except:
           con = psycopg2.connect(host='localhost', database='BD_RFID', user='sinapse', password='napse1si')
           cur = con.cursor()
           query = "SELECT url_server,ipdevice FROM a_serverconfigurations" 
           cur.execute(query)
           resultado = cur.fetchone()
           a_url=str(resultado[0])
           myip=str(resultado[1])
           print("Error peticion getlist:", sys.exc_info()[0])
           open("errorconexion.log", 'w')
           time.sleep(float(refreshtime_error) * 60)
           reiniciaethernet(myip)
           time.sleep(1)		   
			
print("Ejecutamos el script Python")

main()

#t = Thread(target=get_list, args=())
#t.join
