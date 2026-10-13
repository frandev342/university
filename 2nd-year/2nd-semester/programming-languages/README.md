# JUEGO DEL COLGADO EN HASKELL

* En este juego del ahorcado , recibiremos un numero, que de 20 palabras seleccionara una, luego intentaremos poner letras del abecedario, de manera que tenemos que completar esta palabra. SI conseguimos todas las letras GANAMOS

*Caso de fallor los intentos PERDEMOS


Ejm:
* "La palabra en secreto fue Antena
    * a  [<-º]
    * n  [<-º]
    * e  [<-º]
    * h  [<-º]
    * q  [<-º]
    * i  [<-º]
    * p  [<-º]
    * o  [<-º]
    * x  [<-º]
    * perdiste la palbra correcta era: "palabra
Y entre las palabras que se pueden adivinar, nos dara 6 INTENTOS para adivinar la palabra, en caso de ganar o perder , mostrara el mensaje
* A lo maximo recibe 6 saltos de linea [<-º]

COMPILACION:
ghc ahorcadoGame.hs -o ahorcado
