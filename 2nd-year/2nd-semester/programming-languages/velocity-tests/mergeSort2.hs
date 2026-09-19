import GHC.Clock (getMonotonicTimeNSec)
import Control.DeepSeq (force)
import Control.Exception (evaluate)
import Text.Printf

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
    let lista = [100000, 99999 .. 1] :: [Int]

    inicio <- getMonotonicTimeNSec

    resultado <- evaluate (force (mergeSort lista))

    fin <- getMonotonicTimeNSec

    let tiempo = fromIntegral (fin - inicio) / 1e9 :: Double

    printf "Tiempo: %.6f segundos\n" tiempo
