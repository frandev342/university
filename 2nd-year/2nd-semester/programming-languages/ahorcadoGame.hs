module Main where
import Text.Read (readMaybe)

-- Frank Salvador Quico Huamani
-- HONG XIN ANGEL ZHONG CALLASI


listaPalabras :: [String]
listaPalabras = [ "computadora"
                , "montaña"
                , "guitarra"
                , "universo"
                , "tiempo"
                , "libro"
                , "espejo"
                , "silencio"
                , "camino"
                , "estrella"
                , "bosque"
                , "viento"
                , "ciudad"
                , "oceano"
                , "aurora"
                , "historia"
                , "puente"
                , "sombra"
                , "jardin"
                , "ventana"
                ]


match :: String -> String -> String
match xs ys = [if elem x ys then x else '-' | x <- xs]


boolGame :: String -> String -> Int -> IO ()
boolGame secretWord word attempts = do
  let hideWord = match secretWord word
  putStrLn $ "\nWord: " ++ hideWord

  putStrLn $ "Attempts: " ++ show attempts


  if hideWord == secretWord
    then putStrLn "\nYou win! 🎉"

  else if attempts == 0

    then putStrLn $ "\nYou lose. The word was: " ++ secretWord
  else do
    putStr "Introduce a char: "
    letter <- getLine

    case letter of
      [] -> do
        putStrLn "Introduce at least one character."
        boolGame secretWord word attempts
      guess : _ -> do
        let newAttempts = if elem guess secretWord then attempts else attempts - 1
        boolGame secretWord (guess : word) newAttempts

main :: IO ()
main = do
  putStrLn "¡Ahorcado!"
  putStr "Choose a number from 1 to 20: "
  number <- getLine
  case readMaybe number of
    Just n | n >= 1 && n <= length listaPalabras ->
      boolGame (listaPalabras !! (n - 1)) "" 6
    _ -> do
      putStrLn "Choose a valid number from 1 to 20."
      main
