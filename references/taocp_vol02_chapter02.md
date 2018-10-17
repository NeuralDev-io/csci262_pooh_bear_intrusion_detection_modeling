
# Chapter 3   

## D. The exponential distribution   

* After uniform deviates, there is also an *exponenential deviate*. 
* Example:
    * In "arrival time" situations, if a radioactive substance emits alpha particles at a rate such that one particle is emitted every $\mu$ seconds on average, then the time between two successive emissions has the exponential distribution with mean $\mu$.

$$
F(x) = 1 - e^{\frac{-x}{\mu}},~x \geq 0
$$

1) *Logarithm method*   

Clearly, if y$ = F(x) = 1 - e^{\frac{-x}{\mu}}$, then $x = F^{[-1]}(y) = -\mu\log_{e}(1-y)$.  
Therefore $-\mu\log_{e}(1-U)$ has the exponential distribution by Eq. (7).   
Since $1 - U$ is uniformly distributed when $U$ is, we conclude that
$$
X = -\mu\log_{e}U
$$
...is exponentially distributed with mean $\mu$.  

(Note: The case of $U = 0$ must be treated specially; we can substitute any convenient value  $\in$ for $0​$, since the probability of this case is extremely small.)

