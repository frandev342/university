module Main where
cuadrado :: Int -> Int
cuadrado x = x * x

main :: IO ()
main = print (cuadrado 4)
