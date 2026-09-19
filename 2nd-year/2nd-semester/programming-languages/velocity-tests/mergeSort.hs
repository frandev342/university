import System.CPUTime
import Text.Printf
import System.Random

mergeSort :: Ord a => [a] -> [a]
mergeSort [] = []
mergeSort [x] = [x]
mergeSort xs =
    merge (mergeSort izquierda) (mergeSort derecha)
  where
    mitad = length xs `div` 2
    izquierda = take mitad xs
    derecha = drop mitad xs

merge :: Ord a => [a] -> [a] -> [a]
merge [] ys = ys
merge xs [] = xs
merge (x:xs) (y:ys)
    | x <= y  = x : merge xs (y:ys)
    | otherwise = y : merge (x:xs) ys

main :: IO ()
main = do
    let tamaños = [1000, 5000, 10000, 50000, 100000, 500000, 1000000]

    mapM_ medir tamaños

medir :: Int -> IO ()
medir n = do
    let lista = take n (randoms (mkStdGen 42) :: [Int])

    inicio <- getCPUTime

    let resultado = mergeSort lista

    -- Obliga a evaluar el resultado
    print (length resultado)

    fin <- getCPUTime
    let tiempo = fromIntegral (fin - inicio) / 10^12 :: Double
    -- let tiempo = fromIntegral (fin - inicio) / 10^12

    printf "Tiempo de ejecución (n=%d): %.6f segundos\n" n tiempo
