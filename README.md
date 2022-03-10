### Disclamer: This lab is heavily based on the CSE 576 homework that you can find here: ###

#### https://github.com/holynski/cse576_sp20_hw2 ####

I file su cui lavorerete sono `resize_image.cpp`e `filter_image.cpp`. Il
repository funziona come nel caso dell'esercizio 1, potete seguire le stesse
indicazioni per il set-up. In questo esercizio avrete bisogno del file che avete
completato nel primo esercizion, `process_image.cpp`. Copiatelo in nella
castella 'src'. Invece `access_image.cpp`
non ci servirà perché le sue funzionalità sono state inserite in `image.h` nella
struttura Image.

## 1. Image resizing ##

Nella lezione abbiamo parlato del ridimensionamento e interpolazione. In questo
esercizio implementerete i metodi di interpolazione e poi creerete una funzione
che crei una nuova immagine che verrà riempita usandoli.

- Completate `float Image::pixel_nearest(float x, float y, int c) const;`
  contenuto in `src/resize_image.cpp`.
    - Dovrebbe calcolare l'interpolazione nearest neighbour. Ricordate di usare
      l'intero più vicino, non solo il cambio di tipo automatico che in C
      troncherebbe la parte decimale.
    - Nota: fate attenzione ad usare la notazione sulle coordinate usata a
      lezione, o i test non passeranno anche nel caso di un'implementazinoe
      metodologicamente corretta. Usate la funzione
      membro `clamped_pixel(a,b,c)` invece dell'operatore `()`.
- Riempite `Image nearest_resize(const Image& im, int w, int h);. Questo
  dovrebbe:
    - Creare una nuova immagine `w x h` con lo stesso numero di canali di `im
    - Ciclare tutti i pixel e mapparli alle vecchie coordinate
    - Usarer nearest neighbour per riempire l'immagine

Il vostro codice prenderà come input l'immagine:

![small_dog](data/dogsmall.jpg)

e la trasformerà in:

![blocky dog](figs/dog4x-nn.png)

Finito con questa funzione, fate lo stesso con le altre due per l'interpolazione
bilineare:

    float Image::pixel_bilinear(float x, float y, int c) const;
    Image bilinear_resize(const Image& im, int w, int h);

Il risultato finale del test `test_bl_resize` dovrebbe essere questo:

![smooth dog](figs/dog4x-bl.png)

Queste funzioni vanno bene per ingrandire un immagine, ma se vogliamo
rimpicciolirla potremmo avere dei problemi di aliasing, come abbiamo visto a
lezione. Infatti il risultato delle operazioni del test saranno molto rumorose:

![jagged dog thumbnail](figs/dog7th-nn.png)

Come detto a lezione dobbiamo filtrare, prima di ridurre!

## 2. Filtraggio delle immagini usando le convoluzioni ##

Inizieremo filtrando l'immagine con un box filter. Ci sono modi più veloci di
fare questo, ma noi lo implementeremo in maniera naif con una convoluzione,
perché sarà lo stesso modo con cui opereremo i filtri successivi.

### 2.1 Create il vostro box-filter ###

Il box filter che abbiamo visto a lezione può essere rappresentato così:

![box filter](figs/boxfilter.png)

un modo per implementarlo è creare un'immagine, riempirla con 1 e poi
normalizzarla. Quindi la prima funzione che realizzeremo è proprio quella di
normalizzazione.

Completate `void l1_normalize(Image& im)` contenuto in `filter_image.cpp`.
Questo dovrebbe normalizzare la somma dei pixel dell'immagine a 1.

Quindi completate `Image make_box_filter(int w)` contenuto
in `filter_image. cpp`. Useremo sono box-filter quadrati, quindi la dimensione
sarà `w x w` e avrà un solo canale i cui pixel dovranno sommare a 1.

### 2.2 Scrivete la funzione di convoluzioe ###

**Chiamiamo questa funzione convoluzione, ma per essre matematicamente corretti
dovremmo ribaltare il filtro. In realtà stiamo implementando una
cross-correlazione, ma vista la simmetria dei filtri non fa differenza. **
Dovete implementare la formula discussa in classe:

![covolution](figs/convolution.png)

Completate `Image convolve_image(const Image& im, const Image& filter, bool preserve)`
. Per questa funzine ci sono due possibilità. Con le convoluzioni normali
facciamo una somma pesata su un'area dell'immagine. Con più canali teniamo conto
di più possibilità:

- Se il parametro `preserve` è impostato a `true`, la funzioe dovrebbe produrre
  un immagine con lo stesso numero di canali dell'input. Questo è utile per
  esempio, se vogliamo applicare il box-filter ad un'immagine RGB per ottenere
  un'altra immagine RGB. Questo significa che ogni canale verrà filtrato
  separatamente dallo stesso kernel.
- Se `preserve` è a `false` dovremmo ritornare un'immagine con un solo canale
  prodotto applicndo il filtro and ogni canale e poi summandoli insieme in un
  unico canale.

Naturalmente, `filter` dovrebbe avere 1 canale. C'è un `assert` nel codice che
verifica questo. Quando avete finito, testate la convoluzione applicando il
filtro alla nostra immagine (`test_convolution` dentro `test1.cpp`).

L'output dovrebbe essere questo:

![covolution](figs/dog-box7.png)

Ora possiamo effettuare l'operazine di ridiuzione dell'immagine:

![covolution](figs/dogthumb.png)

Potete vedere quanto è migliorata!

Resize                     |  Blur e Resize
:-------------------------:|:-------------------------:
![](figs/dog7th-nn.png)    | ![](figs/dogthumb.png)

### 2.3 Altri filtri ###

Completate le funzioni `Image make_highpass_filter()`
, `Image make_sharpen_filter()`, e `Image make_emboss_filter()`. Il primo lo 
abbiamo visto a lezione, gli altri due sono nuovi.

Highpass                   |  Sharpen                  | Emboss
:-------------------------:|:-------------------------:|:--------------------|
![](figs/highpass.png)     | ![](figs/sharpen.png)     | ![](figs/emboss.png)
:-------------------------:|:-------------------------:|:--------------------|
![](figs/dog-highpass.png)     | ![](figs/dog-sharpen.png)     | ![](figs/dog-emboss.png)

### 2.4 Implementazione del kernel Gaussiano ###

Implementate la funzione `Image make_gaussian_filter(float sigma)` che 
prende una deviazione standard come input e ritorna un filtro gaussiano. 
Quanto dovrebbe essere grande il kernel del filtro? il 99% dell'area di una 
gaussiana si trova a +/- 3 deviazioni stadard, quindi il kernel deve essere 
6 volte sigma. Allo stesso tempo, ricordatevi che il kernel deve avere un 
numero dispari di righe e colonne, quindi sarà 6x sigma + 1.

Dobbiamo riempire i valori del kernel con dei valori. Usate la densità di 
probabilità della gaussiana bidimensionale:

![2d gaussian](figs/2dgauss.png)

Ovviamente questa è un'approssimazione, ma è efficace. Ricordate che la 
simma dei coefficienti del filtro deve essere 1. 

A questo punto il test della funzione di blur dovrebbe funzionare:

![blurred dog](figs/dog-gauss2.png)

## 3. Immagini ibride ##
Come abbiamo visto a lezione, dai filtri passa basso come quello gaussiano è 
possibile ricavare anche quelli passa alto come differenza tra l'immagine 
originale e quella a basse frequenze. Con questa proprietà possiamo fare 
cose interessanti, come ad esempio questo[this tutorial on retouching skin](https://petapixel.com/2015/07/08/primer-using-frequency-separation-in-photoshop-for-skin-retouching/)
in Photoshop.

Possiamo anche produrre immagini molto particolari come queste [really 
trippy images](http://cvcl.mit.edu/hybrid/OlivaTorralb_Hybrid_Siggraph06.pdf)
che sembrano diverse a seconda le guardiamo da vicino o da lontano. 
Quest'ultima cosa è quella che implementeremo. Queste immagini ibride 
prendono l'informazione a bassa frequenza da una immagine e quella ad alta 
frequenza dall'altra. Ecco un esempio:

Small                     |  Medium | Large
:-------------------------:|:-------:|:------------------:
![](figs/marilyn-einstein-small.png)   | ![](figs/marilyn-einstein-medium.png) | ![](figs/marilyn-einstein.png)

Se non credete al ridimensionamento fatto, controllate pure la figura  
figs/marilyn-einstein.png` e guardatela da vicino e da lontano. Il fenomeno 
sarà lo stesso.

Il vostro lavoro e produrre un'immagine simile. Ma invece di riprodurre un 
personaggio celebre trapassato, useremo immagini di personaggi immaginari. 
In particolare sfrutteremo la trama segreta di Harry Potter in cui Silente è 
in realtà Ron Weasly che viaggia nel tempo. Non ci credete? Le immagini non 
mentono!

Small                     | Large
:-------------------------:|:------------------:
![](figs/ronbledore-small.jpg)   | ![](figs/ronbledore.jpg)

Per questa task dovrete estrarre le alte e basse frequenze da alcune 
immagini. Sapete già come ottenere le basse frequenze con il filtro 
gaussiano. Per ottenere le alte dovete sottrarre le basse frequenze 
dall'immagine originale, vale a dire dal campo `data`. 

Completate la funzione `Image add_image(const Image& a, const Image& b)`
e `Image sub_image(const Image& a, const Image& b)` in modo che effettui 
queste trasformazioni. Probabilmente dovrebbero includere alcuni check che 
l'immagine rimanga della stessa dimensione. 
Il risultato prodotto dal test dovrebbe essere:

Low frequency           |  High frequency | Reconstruction
:-------------------------:|:-------:|:------------------:
![](figs/low-frequency.png)   | ![](figs/high-frequency.png) | ![](figs/reconstruct.png)

Avete notato che l'immagine ad alta frequanca va in overflow quando la 
salviamo su disco? È un problema per noi? Perché?

Usate queste funzioni per creare la vostra immagine ibrida. Ci sarà bisogno 
di trovare la giusta deviazione standard perché funzioni. Non esiste un 
valore che vada bene per tutte le immagini. 

## 4. Sobel filters ##

The [Sobel filter](https://www.researchgate.net/publication/239398674_An_Isotropic_3x3_Image_Gradient_Operator)
is cool because we can estimate the gradients and direction of those gradients
in an image. They should be straightforward now that you all are such pros at
image filtering.

### 4.1 Make the filters ###

First implement the functions `make_gx_filter` and `make_gy_filter`
in `filter_image.cpp` to make our sobel filters. They are for estimating the
gradient in the x and y direction:

Gx                 |  Gy
:-----------------:|:------------------:
![](figs/gx.png)   |  ![](figs/gy.png)

### 4.2 One more normalization... ###

To visualize our sobel operator we'll want another normalization
strategy, [feature normalization](https://en.wikipedia.org/wiki/Feature_scaling)
. This strategy is simple, we just want to scale the image so all values lie
between [0-1]. In particular we will
be [rescaling](https://en.wikipedia.org/wiki/Feature_scaling#Rescaling) the
image by subtracting the minimum from all values and dividing by the range of
the data. If the range is zero you should just set the whole image to 0 (don't
divide by 0 that's bad).

### 4.3 Calculate gradient magnitude and direction ###

Fill in the function `pair<Image,Image> sobel_image(const Image& im)`. It should
return two images, the gradient magnitude and direction. The strategy can be
found [here](https://en.wikipedia.org/wiki/Sobel_operator#Formulation). We can
visualize our magnitude using our normalization function(Section 4.1 in
test1.cpp).

![](figs/magnitude.png)

### 4.4 Make a colorized representation ###

Now using your sobel filter try to make a cool, stylized one. Fill in the
function `Image colorize_sobel(const Image& im)`. I used the magnitude as the
saturation and value of an image and the angle (theta) as the hue and then used
our `hsv_to_rgb` function we wrote before. We want the output image to be a
valid image so normalize the range to 0-1 by:

- Feature normalizing the magnitude; and
- Normalizing the angle by dividing by 2π and adding 0.5.

Add some smoothing to the original image (a Gaussian with sigma 4) inside
of `colorize_sobel`, so it looks even nicer.

![](figs/lcolorized.png)

### 4.5 (Extra Credit) Let's blur but slightly differently ###

Now let's try blurring by not just assigning weights to surrounding pixels based
on their spatial location in relation to the center pixel but also by how far
away they are in terms of color from the center pixel. The idea of
the [bilateral filter](https://cs.jhu.edu/~misha/ReadingSeminar/Papers/Tomasi98.pdf)
is to blur everything in the image but the color edges.

Once again we will be forming a filter, except now it will be different per
pixel. The weights for a pixel's filter can be described as such:

![W](figs/bilateral_W.png)

where the individual weights are

![W](figs/bilateral_wij.png)

(the definition for G is above in Section 2.4)

and the normalization factor is

![W](figs/bilateral_norm.png)

for a kernel of size (2k+1).

Hint: For the spatial Gaussian, you can use the `make_gaussian_filter` you
implemented above. For the color distance Gaussian, you should compute the
Gaussian with the distance between the pixel values for each channel separately
and then apply a Gaussian with `sigma2`.

Fill in the
function `Image bilateral_filter(const Image& im, float sigma1, float sigma2)`
where `sigma1` is for the spatial gaussian and `sigma2` is for the color
distance Gaussian. Use a kernel size of `6*sigma1` for the bilateral filter.

Your image should have a similar effect to the image below so we suggest testing
out a few spatial and color sigma parameters before submitting your final image(
you can find the before image in `data/bilateral_raw.png`, note that it is 40x40
pixels and is being upsampled in this README). Good luck!

Before                 |  After
:-----------------:|:------------------:
![](figs/bilateral_pre.png)   |  ![](figs/bilateral_post.png)

![cat](figs/bilateral_cat.png)

## 5. Turn it in ##

Turn in your `resize_image.cpp` and `filter_image.cpp` on Canvas under
Assignment 2.
