import GHC.Clock (getMonotonicTimeNSec)
import Control.DeepSeq (force)
import Control.Exception (evaluate)
import Text.Printf

mergeSort :: Ord a => [a] -> [a]
mergeSort [] = []
mergeSort [x] = [x]
mergeSort xs =
    merge (mergeSort left) (mergeSort right)
  where
    medium = length xs `div` 2
    left = take medium xs
    right = drop medium xs

merge :: Ord a => [a] -> [a] -> [a]
merge [] ys = ys
merge xs [] = xs
merge (x:xs) (y:ys)
    | x <= y    = x : merge xs (y:ys)
    | otherwise = y : merge (x:xs) ys

medir :: Int -> IO ()
medir n = do
    let lista = [n, n-1 .. 1] :: [Int]

    inicio <- getMonotonicTimeNSec

    resultado <- evaluate (force (mergeSort lista))

    fin <- getMonotonicTimeNSec

    let tiempo = fromIntegral (fin - inicio) / 1e9 :: Double

    printf "n = %-7d : %.6f segundos\n" n tiempo

main :: IO ()
main = do
    let tamaños = [1000, 5000, 10000, 50000, 100000, 500000, 1000000]

    mapM_ medir tamaños
