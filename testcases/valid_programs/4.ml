# 斐波那契函数定义

(define (fib x)
    (if (< x 2)
        1
        (+  (fib (- x 1))
            (fib (- x 2))
        )
    )
)

# 快速幂函数
(define (fast_power x base)
    (if (= x 0)
        1
        ((define half_x (/ x 2))
         (define half (fast_power half_x base))
         (define remain (- x (* 2 half_x)))
         (if (= remain 0)
             (* half half) #平方
             (* half half base)
         )
        )
    )
)