// SOCKET ESP32 con consulta/escritura en DB // VERSION SIN TRADUCIR
// IP ESP32: 192.168.1.234

#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>


     // const char *ssid = "INFINITUM729C_2.4";
     // const char *pass = "LaTarjeta123";
      const char *ssid = "RED ACCESA";
      const char *pass = "037E32E7";      
      unsigned long t=0;

   // Variables server
      String texto, buf2; 
      char buf[60],INSERT_SQL[200], evento[100];
      int tam;
      int i = 0;

  // Variables Pic
      String comandoServer="", RESULT;
      char buf_pic[50];

      String  id_s;

      
      
            WiFiServer server;       
            WiFiClient client; 
            MySQL_Connection conn((Client*)&client);
            MySQL_Cursor* cursor;
                  
      
      
            IPAddress server_addr(192,168,1,221);  // IP del server MySQL:  231 - 221
            char user[] = "root";              // MySQL usuario
            char password[] = "ALAala123,.-";        // Contraseña MySQL        

                  
                  ////Obtener la hora/////
                 String get_time(){
                      time_t now;
                      time(&now);
                      char time_output[30];
                      strftime(time_output, 25, "%d/%m/%y %T", localtime(&now)); 
                      return String(time_output); // Formato:   20/Sep/21 12:30:45
                      }
                String HOUR;
 /*
 * ********************************************************************
 *                              Setup 
 * ********************************************************************
*/
  void setup()
          {
              
              Serial.begin(115200);
              Serial.println();
              t=millis();
          
              WiFi.mode(WIFI_STA); 
                           
              configTime(0, 0, "pool.ntp.org", "time.nist.gov");
              setenv("TZ", "CST6CDT,M4.1.0,M10.5.0", 1);

              setup_wifi();         // Iniciamos configuracion del Wifi
              server.begin(132);    // El servidor comienza a escuchar el puerto número 132 
              reconnect_MySQL();    // Conectamos a MySQL
              
              
          } //Ciero el void setup
/*
 * ********************************************************************
 *                              Setup WIFI
 * ********************************************************************
*/

void setup_wifi() {

//  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
//  setenv("TZ", "CST6CDT,M4.1.0,M10.5.0", 1);
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect_MySQL(){
              Serial.print("Connecting to SQL...  ");
              if (conn.connect(server_addr,3306, user, password))
                    Serial.println("SQL OK.");
              else
                    Serial.println("SQL FAILED.");

                  //create MySQL cursor object
              cursor = new MySQL_Cursor(&conn);  
 
              delay(1000);
    }
/*
 * ********************************************************************
 *                              Void LOOP
 * ********************************************************************
*/
  void loop(){   
              if (!conn.connected()) {  
                Serial.println("[MySQL no conectado, intentaremos reconectar]");
                reconnect_MySQL();  
              }


              
              WiFiClient client = server.available(); // Intenta crear un objeto de cliente
              if (client) // Si el cliente está disponible
              {
               Serial.println("[Cliente conectado]");
               
                  while (client.connected()) // Si el cliente está conectado
                  {

                                while (millis()-t>=1500)
                                {                                
                                client.print("!");  // Envío heartbeat
                                Serial.print("!");
                                
                                
                                i = i+1;
                                if (i == 10){
                                  i = 0;
                                  client.stop();
                                  server.begin(132);
                                  //ESP.restart();
                                  
//                                  client.stop();
//                                  WiFi.disconnect();
//                                  setup_wifi();
//                                  //WiFi.reconnect();
//                                  server.begin(132);
//                                  reconnect_MySQL(); 
                                }
                                                             
                                TimerServerSQLRead (); //}// Timer para leer DB cada segundo
                                        
                                        //RESULT.replace("<","<TPV");
                                        client.println(RESULT);
                                        
                                          
                                         
                                           if (RESULT !=0)
                                          {              
                                            Serial.println(RESULT);
                                            MySQL_Cursor *cur_update = new MySQL_Cursor(&conn);
                                            //String update_tabla = "UPDATE conacclatarjeta.eventosPC SET fechaAtencion = '" + get_time() + "', estatus = 'leido' WHERE id = " + id_s +";";
                                            String update_tabla = "UPDATE conacclatarjeta.eventosPC SET fechaAtencion = '" + get_time() + "', estatus = 'leido' WHERE id = " + id_s +";";                                            
                                            char cuery_UP [200];
                                            update_tabla.toCharArray(cuery_UP,200);
                                            cur_update->execute(cuery_UP);

//                                              if (result.indexOf("RST")==4 || result.indexOf("RST")==7 )
//                                              {                                               
//                                              ESP.restart(); 
//                                              }
                                              
                                              Serial.println("DB: Evento actualizado ");
                                              
                                          }
                                          else if (RESULT==""){
                                            ///
                                
                                          }    
                                                RESULT="";
                                                    
                                       //////////***********///////////**********///////////********

                                t=millis();  
                                   }
                                        
                                if (client.available()) {
                            
                                    tam= client.readBytesUntil('\n',buf, 60);  //Para leer el numero de bytes
                                    buf[tam]='\0';                             
                                    texto = String(buf);   //String
                                    
                                           
//////////////////////////////////////////////////////   PROCESO   /////////////////////////////////////////////////////////////////
                           
                           if (texto.indexOf("TPV")>=1)
                           {
                                /* 
                                 *  **********************************************************************************************
                                */
                                //  Relizaremos un intento de coneccion para ver que todo este correcto
                                // en caso de no estar conectado, se volvera a conectar
                                if (!conn.connected()) {  
                                      Serial.println("[MySQL no conectado, dejo de funcionar en el loop]");
                                      reconnect_MySQL();  
                                }
                                i = 0;
                                
                                ///////////
                                
                                String bufTPV;
                                char INSERT_SQL_TPV[200];
                                texto.replace("<>", "");
                                
                                //bufTPV = "INSERT INTO conacclatarjeta.eventosfrompic(fechaRegistro, evento, estatus) VALUES ( '"+ get_time() +"','"+texto+"', 'sin leer')";          
                                bufTPV = "INSERT INTO conacclatarjeta.eventosfrompic(fechaRegistro, evento, estatus) VALUES ( '"+ get_time() +"','"+texto+"', 'sin leer')";          
                                bufTPV.toCharArray(INSERT_SQL_TPV,200);
                                Serial.println(bufTPV);
                                
                                if (conn.connected()){
                                cursor->execute(INSERT_SQL_TPV);}                                                              
                                
                                //texto.replace("TPV", "");                                 
                                //Serial.println(texto);
                                       
//                                ParcheToS();

                                buf2 = "INSERT INTO conacclatarjeta.eventospic(fechaRegistro, fechaAtencion, evento, estatus) VALUES ( '"+ get_time() +"', '' ,'"+texto+"', 'sin leer')";          
                                buf2.toCharArray(INSERT_SQL,200);
                                Serial.println(INSERT_SQL);   
                                
                                if (conn.connected()){
                                cursor->execute(INSERT_SQL);
                                     }

                                client.print("<SQLWRT1>");
                                delay(200);     
                                               
                           }


//////////////////////////////////////////                                   
                                                   
                                 } // Se cierra el client.available 
                          } // Se cierra el client.connected
                                                                                
                // Si el cliente se desconecta 
                  client.stop();
                  Serial.println();
                  Serial.println("[Cliente desconectado]");


   /// AQUI TERMINA EL PROGRAMA PRINCIPAL 

   
  
  
                       } //Se cierra el client
                  } // Se cierra el Loop
  
  
  
// FUNCIONES :   
  void TimerServerSQLRead ()  
       {
                         
                          row_values *row = NULL;
                          long head_count=0, head_countEv = 0;
                          id_s="";
                          MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);    
                                                            
                          char id[80]="SELECT id FROM conacclatarjeta.eventosPC WHERE estatus = 'sin leer';"; //query                           
                          cur_mem->execute(id);

                     ////////////////////////////////////////////////////////////////  
                                  column_names *cols = cur_mem->get_columns();
                                  do {
                                   row = cur_mem->get_next_row();
                                   if (row != NULL) {
                                   head_count = atol(row->values[0]);
                                   }
                                   } while (row != NULL);
                                    //Deleting the cursor also frees up memory used
                                   delete cur_mem;
                     ////////////////////////////////////////////////////////////////

                                 int id_i;
 
                                  id_s= String(head_count);
                                  id_i= id_s.toInt();


                            if (id_i > 0 )
                            {
                               row_values *rowID = NULL;
                               long head_countEv = 0;
                               MySQL_Cursor *cur_memID = new MySQL_Cursor(&conn);
                               String row_ID;
                                
                               String evento_string= "SELECT evento FROM conacclatarjeta.eventosPC WHERE estatus = 'sin leer' AND id = '"+id_s+"';";
                               evento_string.toCharArray(evento,100);
                               cur_memID->execute(evento);
                                  
                     ////////////////////////////////////////////////////////////////

                                  column_names *colsID = cur_memID->get_columns();                                  
                                  do {
                                   rowID = cur_memID->get_next_row();
                                   if (rowID != NULL) {
                                   row_ID= rowID->values[0]; 
                                   }
                                   } while (rowID != NULL);
                                   delete cur_memID;
                     ////////////////////////////////////////////////////////////////

                                   row_ID.toCharArray(buf_pic, 50) ; //<-- BUF_PIC : Guarda el valor del evento en Char (Ex. RESET_TPV )
                                   RESULT = row_ID; 
              

                              }

                
                } // Se cierra el TimerServerSQLRead
