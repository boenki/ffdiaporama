<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="ru_RU">
<context>
    <name>0010</name>
    <message>
        <source>How ffDiaporama works</source>
        <translation>Принципы работы ffDiaporama</translation>
    </message>
    <message>
        <source>Functional principles</source>
        <translation>Принципы функционирования</translation>
    </message>
    <message>
        <source>Predefined title slide</source>
        <translation>Шаблоны заголовков слайдов</translation>
    </message>
    <message>
        <source>The &quot;Visible&quot; property of blocks and the management of the breaks</source>
        <translation>Свойство &quot;видимости&quot; блоков и управление переходами между ними</translation>
    </message>
    <message>
        <source>All shots regulations versus current shot regulations</source>
        <translation>Настройки всех кадров и настройки текущего кадра</translation>
    </message>
    <message>
        <source>Style sheets</source>
        <translation>Таблицы стилей</translation>
    </message>
    <message>
        <source>ffDiaporama WIKI - How ffDiaporama works</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This section contains general information on the concepts and methods of use of ffDiaporama </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Index</source>
        <translation type="unfinished">Содержание</translation>
    </message>
    <message>
        <source>Dynamic variables</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0011</name>
    <message>
        <source>Functional principles</source>
        <translation>Приницпы функционирования</translation>
    </message>
    <message>
        <source>The main control element of ffDiaporama is the timeline. The timeline lists the slides which compose the project.</source>
        <translation>Основным элементом в ffDiaporama является линия времени (таймлайн). На ней располагаются слайды, которые составляют проект.</translation>
    </message>
    <message>
        <source>It consists of 3 tracks: </source>
        <translation>Она состоит из 3 треков: </translation>
    </message>
    <message>
        <source>the background track</source>
        <translation>трек фоновых изображений</translation>
    </message>
    <message>
        <source>the mounting track</source>
        <translation>трек монтажа</translation>
    </message>
    <message>
        <source>and the music track.</source>
        <translation>трек музыки.</translation>
    </message>
    <message>
        <source>The slides</source>
        <translation>Слайды</translation>
    </message>
    <message>
        <source>Slides are shown one after another, in the order in which they appear in the time line.</source>
        <translation>Слайды отображаются один за другим в том порядке, в котором они появляются на линии времени.</translation>
    </message>
    <message>
        <source>The following diagram shows how slides function:</source>
        <translation>Следующая схема наглядно демонстрирует принципы функционирования слайдов:</translation>
    </message>
    <message>
        <source>The upper part of the diagram represents four slides: A, B, C and D. </source>
        <translation>В верхней части схемы представлено четрые слайда: A, B, C, D. </translation>
    </message>
    <message>
        <source>Each slide possesses an entering transition (symbolized on the diagram by a small rectangle labelled T). The transitions can have different durations (symbolized on the diagram by the lengths TA&apos;, TB&apos;, etc.): It is possible to define the duration of the entering transition for each slide separately (or to indicate no transition: duration=0).</source>
        <translation>Каждый слайд имеет входящий переход (передставлен на схеме как маленький прямоугольник, помеченный буквой T). Переходы могут быть разной продолжительности (представлены на схеме отрезками TA, TB и т.д.): можно задать длительность входящего перехода для каждого слайда отдельно (или вообще убрать переход: длительность= 0).</translation>
    </message>
    <message>
        <source>Each slide possesses a duration which is appropriate for it (symbolized on the diagram by the lengths B&apos;, C&apos;, etc.): Some last only a few seconds while others can last several minutes. The durations are expressed in seconds but are not necessarily whole numbers: a slide might last 12.234 seconds.</source>
        <translation>Каждый слайд имеет соответствующую продолжительность (отмечено на схеме отрезками B&apos;, C&apos;, и т.д.): некоторые могут длиться всего несколько секунд, в то время как другие - несколько минут. Продолжительность выражается в секундах, но не обязательно целыми значениями. Например, слайд может длиться 12,234 секунды.</translation>
    </message>
    <message>
        <source>Each slide is a composite, an assembly of one or more multimedia elements which are animated over time. The animation is realized through shots.</source>
        <translation>Каждый слайд представляет собой композицию из одного или нескольких мультимедийных элементов, которые анимированны во времени. Анимации осуществляется посредством кадров.</translation>
    </message>
    <message>
        <source>Each slide can define a new background (that is shown under the multimedia elements of the slide if they don&apos;t fill out the whole screen).</source>
        <translation>Каждый слайд может иметь свой фон, который отображается под мультимедийными элементами слайда, если они заполняют не весь экран.</translation>
    </message>
    <message>
        <source>Each slide can define a new musical play-list.</source>
        <translation>Каждый слайд может иметь свой список воспроизведения музыки (плейлист).</translation>
    </message>
    <message>
        <source>Slides which do not define a background use the background defined in a previous slide. So in the following example: </source>
        <translation>Слайды для которых не определен фон используют фоновое изображение, определенное для предыдущего слайда. Например:</translation>
    </message>
    <message>
        <source>Slide 1: Defines a background</source>
        <translation>Слайд 1: определен фон</translation>
    </message>
    <message>
        <source>Slide 2: Does not define a background and therefore uses the background defined by slide 1</source>
        <translation>Слайд 2: фон не определен, поэтому используется фон, определенный для слайда 1</translation>
    </message>
    <message>
        <source>Slide 3: Does not define a background and therefore uses the background defined by slide 1</source>
        <translation>Слайд 3: фон не определен, поэтому используется фон, определенный для слайда 1</translation>
    </message>
    <message>
        <source>Slide 4: Defines a background</source>
        <translation>Слайд 4: определен фон</translation>
    </message>
    <message>
        <source>Slide 5: Does not define a background and therefore uses the background defined by slide 4</source>
        <translation>Слайд 5: фон не определен, поэтому используется фон, определенный для слайда 4</translation>
    </message>
    <message>
        <source>The same principle applies to the musical play-lists. As long as slides do not define a new play-list, they use the last defined play-list.</source>
        <translation>Этот же принцип используется при воспроизведении музыкальных файлов. До тех пор, пока для какого-то слайда не определен свой плейлист, используется последний определенный плейлист.</translation>
    </message>
    <message>
        <source>Remarks:</source>
        <translation>Примечания:</translation>
    </message>
    <message>
        <source>The total duration of the project is not equal to the sum of the durations of the slides because the transitions make the slides overlap: Two slides of 10 seconds, each with a transition of 1 second, will give a total duration for the project of 19 seconds (and not 20 seconds)!</source>
        <translation>Общая продолжителньность проекта не равна сумме продолжительностей составляющих его слайдов, т.к. переходы обеспечивают перекрытие слайдов: два слайда по10 секунд, каждый с переходом в 1 секунду соответствуют общей продолжительности проекта в 19 секунд (а не 20 секунд)!</translation>
    </message>
    <message>
        <source>Shots and blocks</source>
        <translation>Кадры и блоки</translation>
    </message>
    <message>
        <source>The lower part of the diagram represents the composition of slide C. </source>
        <translation>В нижней части схемы представлен слайд С.</translation>
    </message>
    <message>
        <source>Each slide comprises one or more shots. The shots are represented on the diagram by rectangles C1, C2, C3, etc...</source>
        <translation>Каждый слайд состоит из одного или нескольких кадров. Кадры отмечены на схеме прямоугольниками C1, C2, C3, и т.д...</translation>
    </message>
    <message>
        <source>Each shot has its own duration (symbolized on the diagram by the lengths C1 &apos;, C2 &apos;, C3 &apos;, etc.). Some shots can last only seconds while others can last several minutes. Some shots can have a duration of zero. (The durations are expressed in seconds but are not necessarily whole numbers: A shot can very well last 12.234 seconds).</source>
        <translation>Каждый кадр имеет собственную продолжительность (отмечено на схеме отрезками C1 &apos;, C2 &apos;, C3 &apos; и т.д...). Некоторые кадры могут длиться всего несколько секунд, в то время как другие - несколько минут. Продолжительность некоторых кадров  может быть равна 0. (Продолжительность выражается в секундах, но не обязательно целыми значениями. Например, кадр может длиться 12,234 секунды).</translation>
    </message>
    <message>
        <source>The shots can contain several multimedia objects. The multimedia objects are integrated into blocks which can be of three types: title (containing only text), images (or photos), or video.</source>
        <translation>Кадры могут содержать несколько мультимедийных объектов, которые могут быть трех типов: заголовок (содержит только текст), изображение (или фотография) и видео.</translation>
    </message>
    <message>
        <source>The blocks define where and how the multimedia object appears on the screen. Each block can have a different framing that defines what is visible from the multimedia object within the block.</source>
        <translation>Блоки определяют, где и как мультимедийные объекты появятся на экране. Каждый блок может быть разной формы, которая определяет какая часть мультимедийного объекта видна.</translation>
    </message>
    <message>
        <source>The background of the shots is transparent. This means that if the image does not fill the whole screen, the background will appear.</source>
        <translation>Фон кадра является прозрачным. Это значит, что если изображение заполняет не  весь экран, то у него появляется фон.</translation>
    </message>
    <message>
        <source>The shots define the arrangement of blocks (multimedia objects) at any given moment.</source>
        <translation>Кадры определяют расположение блоков (мультимедийных объектов) в данный момент.</translation>
    </message>
    <message>
        <source>The shot animations are generated by the transformation of these arrangements from one shot to another.</source>
        <translation>Анимация кадров создается путем изменения их расположения на одном кадре по сравнению с другим.</translation>
    </message>
    <message>
        <source>The lengths of the shot animations are defined by the shot durations.</source>
        <translation>Длительность анимации кадра определяется его продолжительностью.</translation>
    </message>
    <message>
        <source>Specifically, if in the foreground, a block containing an image occupies the whole screen, and then in the following shot the same block occupies only a small part of the screen, then the animation will consist of the transformation of the image from the whole screen into that small part of the screen.</source>
        <translation>В частности, если на переднем плане находится блок с изображением, занимающим весь экран, а на следующем кадре тот же блок занимают лишь небольшую часть экрана, то анимация будет состоять в уменьшении полноэкранного изображения до соответствующего размера.</translation>
    </message>
    <message>
        <source>Example:</source>
        <translation>Пример:</translation>
    </message>
    <message>
        <source>Let&apos;s have 3 shots within a slide:</source>
        <translation>Представим себе слайд, содержащий 3 кадра:</translation>
    </message>
    <message>
        <source>Shot 1: Image using the whole screen: Duration=2 seconds.</source>
        <translation>Кадр 1: полноэкранное изображение; продолжительность 2 секунды.</translation>
    </message>
    <message>
        <source>Shot 2: Image using only the bottom right screen quarter: Duration 3 seconds.</source>
        <translation>Кадр 2: изображение, занимающее только нижнюю правую четверть экрана; продолжительность 3 секунды.</translation>
    </message>
    <message>
        <source>Shot 3: Image again using the whole screen: Duration=2 seconds.</source>
        <translation>Кадр 3: изображение снова занимает весь экран; продолжительность 2 секунды.</translation>
    </message>
    <message>
        <source>Result:</source>
        <translation>Результат:</translation>
    </message>
    <message>
        <source>The image will be shown on the whole screen for 2 seconds, will shrink gradually over 3 seconds towards the right lower quarter of the screen, and then will grow again for 2 seconds until it once more occupies the whole screen.</source>
        <translation>Изображение будет показано на весь экран 2 секунды, затем в течение 3 секунд будет постепенно уменьшаться до нижней правой четверти экрана, а затем за 2 секунды опять увеличится до полноэкранного.</translation>
    </message>
    <message>
        <source>The slide will end as soon as the image has reached the position defined in shot 3.</source>
        <translation>Слайд завершится, как только изображение достигнет позиции, определенной в третьем кадре.</translation>
    </message>
    <message>
        <source>The transformations are gradual: in the example you will see the image being gradually reduced.</source>
        <translation>Преобразование происходит постепенно: в этом примере вы увидите, что изображение уменьшается постепенно, а не мгновенно.</translation>
    </message>
    <message>
        <source>The transformations are made over the whole duration of the following shot.</source>
        <translation>Преобразования выполняется за время соответствующее продолжительности следующего кадра.</translation>
    </message>
    <message>
        <source>The framing definition of the objects in each block can vary from one shot to another. The following example shows three different definitions from the same block:</source>
        <translation>Обрезка (форма) объектов в каждом блоке может изменяться от одного кадра к другому. Следующий пример продемонстрирует три различных положения одного и того же блока:</translation>
    </message>
    <message>
        <source>1st shot: The image is complete.</source>
        <translation>1-ый кадр: целое изображение.</translation>
    </message>
    <message>
        <source>2nd shot: The image is re-framed on the statue.</source>
        <translation>2-ой кадр: изображение обрезается до фрагмента со статуей.</translation>
    </message>
    <message>
        <source>3rd shot: The image is re-framed on the pedestrians.</source>
        <translation>3-ий кадр: изображение обрезается до фрагмента с пешеходами.</translation>
    </message>
    <message>
        <source>If we apply these framing definitions to the previous 3 shots: </source>
        <translation>Если мы применим такую обрезку к предыдущим 3 кадрам:</translation>
    </message>
    <message>
        <source>The complete image will be shown on all the screen for 2 seconds, then it will be reduced gradually towards the right lower quarter of the screen over 3 seconds while zooming in on the statue, then it will grow again over 2 seconds until it once more occupies the whole screen while the &quot;camera&quot; moves towards the pedestrians.</source>
        <translation>Полное изображение будет показано на весь экран в течение 2 секунд, затем оно в течение 3 секунд будет постепенно уменьшаться до нижней правой четверти экрана, пока не покажет только статую, а затем в течение 2 секунд будет увеличиваться, пока снова не займет весь экран. Получается, как бы, что камера движется навстречу пешеходам.</translation>
    </message>
    <message>
        <source>ffDiaporama can manage up to several hundreds slides in a single project.</source>
        <translation>ffDiaporama может работать с несколькими сотнями слайдов в одном проекте.</translation>
    </message>
    <message>
        <source>Each slide can consist of tens of shots.</source>
        <translation>Каждый слайд может содержать десятки кадров.</translation>
    </message>
    <message>
        <source>Each slide can contain tens of blocks.</source>
        <translation>Каждый слайд может содержать десятки блоков.</translation>
    </message>
    <message>
        <source>Blocks can be visible on some shots and invisible on other.</source>
        <translation>Блоки могут быть видимыми на одних кадрах и не видимыми на других.</translation>
    </message>
</context>
<context>
    <name>0012</name>
    <message>
        <source>Predefined title slide</source>
        <translation>Шаблон заголовка слайда</translation>
    </message>
</context>
<context>
    <name>0013</name>
    <message>
        <source>Dynamic variables</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0014</name>
    <message>
        <source>The &quot;Visible&quot; property of blocks and the management of the breaks</source>
        <translation>Свойство &quot;видимости&quot; блоков и управление переходами между ними</translation>
    </message>
    <message>
        <source>Blocks have a visible/invisible property. In the following example, the one image is inserted 3 times (in 3 different blocks) into the same slide.</source>
        <translation>Блоки имеют свойство видимости / невидимости. В следующем примере одно изображение вставляется 3 раза (в 3 разных блока) на один и тот же слайд.</translation>
    </message>
    <message>
        <source>The first block consists of the whole image filling the whole screen</source>
        <translation>Первый блок состоит целиком из полноэкранного изображения</translation>
    </message>
    <message>
        <source>The second block consists of the image re-framed on the statue and positioned in an ellipse at the right of the screen</source>
        <translation>Второй блок содержит изображение, обрезанное в форме эллипса до фрагмента со статуей, размещенное в правой части экрана</translation>
    </message>
    <message>
        <source>The third block consists of the image re-framed on the pedestrians and also positioned in an ellipse at the right of the screen</source>
        <translation>Третий блок содержит изображение, обрезанное в форме эллипса до фрагмента с пешеходами, размещенное в правой части экрана</translation>
    </message>
    <message>
        <source>The statue and the pedestrians have their visibility property set to invisible on the 1st shot.</source>
        <translation>Статуе и пешеходам установлено свойство невидимости на первом кадре.</translation>
    </message>
    <message>
        <source>Then the statue becomes visible on the 2nd shot.</source>
        <translation>Затем на втором кадре статуя мтановится видимой.</translation>
    </message>
    <message>
        <source>Finally, the statue once more becomes invisible on the 3rd shot and the pedestrians become visible.</source>
        <translation>Наконец, на третьем кадре статуя снова становится невидимой и видимыми становятся пешеходы.</translation>
    </message>
    <message>
        <source>Result:</source>
        <translation>Результат:</translation>
    </message>
    <message>
        <source>For 2 seconds, the 1st shot is shown and we see only the whole image which occupies all the screen</source>
        <translation>2 секунды показывается первый кадр и мы видим только целое изобржение, занимающее полный экран</translation>
    </message>
    <message>
        <source>Over the next 2 seconds, the statue appears</source>
        <translation>В течение следующих 2 секунд появляется статуя</translation>
    </message>
    <message>
        <source>Over a further 2 seconds, the statue is replaced by the pedestrians</source>
        <translation>В течение следующих 2 секунд статуя заменяется пешеходами</translation>
    </message>
    <message>
        <source>This example is really very simple, because it is also possible to animate the appearance and the disappearance of blocks.</source>
        <translation>Этот пример является очень простым, поскольку также возможна анимация появления и исчезновения блоков.</translation>
    </message>
    <message>
        <source>Indeed, as shown previously, animations are generated by the transformation of the arrangement from one shot to another.</source>
        <translation>Итак, как и упоминалось ранее, анимация создается  путем изменения расположения на одном кадре по сравнению с другим.</translation>
    </message>
    <message>
        <source>So, if on the 1st shot, the statue occupied the left lower quarter of the screen rather than the same position as on the 2nd shot, then instead of simply appearing for 2 seconds on the 2nd shot, it would appear by growing out of the left lower quarter of the screen.</source>
        <translation>Таким образом, если на первом кадре статуя занимала левую нижнюю четверть экрана, а на втором кадре иную позицию, то вместо простого появления через 2 секунды на 2 кадре она будет увеличиваться из левой нижней четверти экрана.</translation>
    </message>
    <message>
        <source>Using videos:</source>
        <translation>Использование видео:</translation>
    </message>
    <message>
        <source>You should note that videos are paused when they are invisible.</source>
        <translation>Следует отметить, что видео становится на паузу , когда оно невидимо.</translation>
    </message>
    <message>
        <source>Therefore a video can be displayed for several seconds in the 1st shot, then be made invisible and thus paused in a 2nd shot, then be visible in a 3rd shot and resume playing, from where it stopped at the end of the 1st shot.</source>
        <translation>Таким образом,видео может появиться в течение нескольких секунд на первом кадре, затем стать невидимым и поставиться на паузу на втором кадре, а затем снова стать видимым на третьем кадре и продолжить воспроизводиться с того места, где оно остановилось в конце первого кадра.</translation>
    </message>
</context>
<context>
    <name>0015</name>
    <message>
        <source>All shots regulations versus current shot regulations</source>
        <translation>Настройки всех кадров и настройки текущего кадра</translation>
    </message>
    <message>
        <source>&lt;B&gt;The settings which apply to the current shot.&lt;/B&gt; These settings are made shot by shot. So, every shot possesses its own settings and the passage from a shot to the other one is defined by the passage from one setting to an other one creating a shot animation: these are called Ken Burns effects. Here are some some: </source>
        <translation>&lt;B&gt;Настройки, которые применяются к текущему кадру.&lt;/B&gt; Эти настройки применяются кадр за кадром. Так,  каждый кадр имеет свои собственные настройки и переход от одного кадра к другому сопровождается переходом от одних настроек к другим, создаваю короткую анимацию: их называют эффектами Кена Бернса. Вот некоторые из них:</translation>
    </message>
    <message>
        <source>Zoom on a portion of the image</source>
        <translation>Увеличение части изображения</translation>
    </message>
    <message>
        <source>Scrolling from a portion of the image to an other one</source>
        <translation>Прокрутка от одной части изображения к другой</translation>
    </message>
    <message>
        <source>Clarification or darkening of a portion of the image</source>
        <translation>Осветление или затемнение части изображеия</translation>
    </message>
    <message>
        <source>etc...</source>
        <translation>и др...</translation>
    </message>
    <message>
        <source>There are two types of settings in the &lt;a href=&quot;0119.html&quot;&gt;Slide properties dialog&lt;/a&gt;:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;The settings which apply to all shots.&lt;/B&gt; These settings are made globally, that is their effects will be visible on all the shots of the slide. Here are some: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Text and text format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shape form</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0016</name>
    <message>
        <source>Style sheets</source>
        <translation>Таблицы стилей</translation>
    </message>
    <message>
        <source>ffDiaporama uses a style sheet system to facilitate the settings for various things.</source>
        <translation>ffDiaporama использует систему таблицы стилей для облегчения настройки различных вещей.</translation>
    </message>
    <message>
        <source>Style sheets exist for:</source>
        <translation>Таблицы стилей существуют для:</translation>
    </message>
    <message>
        <source>Controlling the block coordinates, including the parameters of size, position and block rotation, and also the framing style.</source>
        <translation>Управления координатами блока, которое включает в себя размер, положение и вращение блока, а также стиль кадрирования.</translation>
    </message>
    <message>
        <source>Controlling the block shape, which includes the parameters of form, opacity, shadow and block borders.</source>
        <translation>Управления формой блока, которое включает в себя тип формы, прозрачность, тени и границы блока.</translation>
    </message>
    <message>
        <source>Controlling image framing, which includes the parameters of geometrical constraint and size.</source>
        <translation>Управление кадрированием изображения, которое включает в себя ограничения геометрии и размер.</translation>
    </message>
    <message>
        <source>Formatting text, which includes the parameters of font, character attributes, alignment and text shadow.</source>
        <translation>Форматирование текста, которое включает в себя шрифт, атрибуты символов, выравнивание текста и тень.</translation>
    </message>
    <message>
        <source>Controlling text background, which includes the parameters of background (color, brush, etc.).</source>
        <translation>Управление фоном текста, которое включает в себя тип фона (цвет, кисть и др.).</translation>
    </message>
    <message>
        <source> Open the style menu:</source>
        <translation>Открытие меню стилей:</translation>
    </message>
    <message>
        <source>The first part of the menu lists the known styles. </source>
        <translation>Первая часть меню содержит список существующих стилей.</translation>
    </message>
    <message>
        <source>If the current settings correspond to a known style, this style is marked with an asterisk.</source>
        <translation>Если текущие параметры соответствуют существующему стилю, то этот стиль помечен звездочкой.</translation>
    </message>
    <message>
        <source>The standard styles of ffDiaporama are marked with this icon: </source>
        <translation>Стандартные стили ffDiaporama помечены этим значком:</translation>
    </message>
    <message>
        <source>Standard styles that you have modified and styles which you created are marked with this icon: </source>
        <translation>Стандартные стили, которые вы изменили и стили, которые вы создали, помечены этим значком:</translation>
    </message>
    <message>
        <source>The second part of the menu allows you to modify the style list. </source>
        <translation>Вторая часть меню позволяет изменять список стилей.</translation>
    </message>
    <message>
        <source>Create a new style: allows you to create a new style with the current settings. When you select this option, a dialog appears asking you to name the new style.</source>
        <translation>Создание нового стиля: позволяет создать новый стиль с текущими параметрами. При выборе этой опции появится диалоговое окно с запросом имени нового стиля.</translation>
    </message>
    <message>
        <source>Modify an existing style: allows you to change the settings of an existing style by using the current settings. When you select this option, the list of known styles appears. Select the style to be modified.</source>
        <translation>Изменение существующего стиля: позволяет изменять параметры существующего стиля на основе текущих параметров. При выборе этой опции появится список известных стилей. Выберите стиль для изменения.</translation>
    </message>
    <message>
        <source>Manage styles: opens the Manage style dialog.</source>
        <translation>Управлением стилями: открывает диалоговое окно Управление стилем.</translation>
    </message>
</context>
<context>
    <name>0020</name>
    <message>
        <source>Interface description</source>
        <translation>Описание интерфейса</translation>
    </message>
    <message>
        <source>The main window</source>
        <translation>Главное окно</translation>
    </message>
    <message>
        <source>The toolbar</source>
        <translation>Панель инструментов</translation>
    </message>
    <message>
        <source>The timeline</source>
        <translation>Линия времени (таймлайн)</translation>
    </message>
    <message>
        <source>The multimedia file browser</source>
        <translation>Мультимедиа Проводник</translation>
    </message>
    <message>
        <source>Application options</source>
        <translation>Настройки программы</translation>
    </message>
    <message>
        <source>Manage devices</source>
        <translation>Управление устройствами</translation>
    </message>
    <message>
        <source>Add or modify a predefined title slide</source>
        <translation>Добавление или изменение слайда из шаблона</translation>
    </message>
    <message>
        <source>Background properties</source>
        <translation>Свойства фона</translation>
    </message>
    <message>
        <source>Chapter properties</source>
        <translation>Свойства главы</translation>
    </message>
    <message>
        <source>Check configuration</source>
        <translation>Проверка конфигурации</translation>
    </message>
    <message>
        <source>Export project</source>
        <translation>Экспорт проекта</translation>
    </message>
    <message>
        <source>Project properties</source>
        <translation>Свойства проекта</translation>
    </message>
    <message>
        <source>Select files</source>
        <translation>Выбор файлов</translation>
    </message>
    <message>
        <source>Correct, reframe or cut image or video</source>
        <translation>Коррекция, кадрирование или обрезка изображения или видео</translation>
    </message>
    <message>
        <source>File information</source>
        <translation>Информация о файле</translation>
    </message>
    <message>
        <source>Manage favorite</source>
        <translation>Управление избранным</translation>
    </message>
    <message>
        <source>Manage style</source>
        <translation>Управление стилями</translation>
    </message>
    <message>
        <source>Music properties</source>
        <translation>Параметры музыки</translation>
    </message>
    <message>
        <source>Render video</source>
        <translation>Создание видео</translation>
    </message>
    <message>
        <source>Ruler properties</source>
        <translation>Свойства линеек</translation>
    </message>
    <message>
        <source>Edit thumbnail model</source>
        <translation>Изменение шаблона обложки</translation>
    </message>
    <message>
        <source>Set first shot duration for a set of slides</source>
        <translation>Выбор продолжительности первого кадра для группы слайдов</translation>
    </message>
    <message>
        <source>Slide properties</source>
        <translation>Параметры слайда</translation>
    </message>
    <message>
        <source>Edit text</source>
        <translation>Редактирование текста</translation>
    </message>
    <message>
        <source>Select transition duration for a set of slides</source>
        <translation>Выбор продолжительности перехода для группы слайдов</translation>
    </message>
    <message>
        <source>Transition properties</source>
        <translation>Параметры перехода</translation>
    </message>
    <message>
        <source>Dialog boxes</source>
        <translation>Диалоговые окна</translation>
    </message>
    <message>
        <source>ffDiaporama WIKI - Interface description</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This section contains information about using ffDiaporama. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Index</source>
        <translation type="unfinished">Содержание</translation>
    </message>
</context>
<context>
    <name>0021</name>
    <message>
        <source>The main window</source>
        <translation>Главное окно</translation>
    </message>
    <message>
        <source>The main window of ffDiaporama allows all the common tasks of slide management.</source>
        <translation>Главное окно ffDiaporama предназначено для выполнения всех общих задач управления слайдами.</translation>
    </message>
    <message>
        <source>The general principle is:</source>
        <translation>Основной принцип:</translation>
    </message>
    <message>
        <source>Add files and titles to the &lt;a href=&quot;0023.html&quot;&gt;timeline&lt;/a&gt; in the form of slides</source>
        <translation>Добавление файлов и заголовков на &lt;a href=&quot;0023.html&quot;&gt; линию времени (таймлайн)&lt;/a&gt; в виде слайдов</translation>
    </message>
    <message>
        <source>Organize slides</source>
        <translation>Организация слайдов</translation>
    </message>
    <message>
        <source>Adjust slides and create animations</source>
        <translation>Управление слайдами и создание анимации</translation>
    </message>
    <message>
        <source>To manage slides, ffDiaporama provides three display modes:</source>
        <translation>Для управления слайдами ffDiaporama предлагает три режима работы:</translation>
    </message>
    <message>
        <source>File mode</source>
        <translation>Режим менеджера файлов</translation>
    </message>
    <message>
        <source>In this mode, the main window is divided into 3 zones: </source>
        <translation>В этом режиме главное окно разделено на 3 зоны:</translation>
    </message>
    <message>
        <source>The &lt;a href=&quot;0023.html&quot;&gt;timeline&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;0023.html&quot;&gt;Линия времени&lt;/a&gt;</translation>
    </message>
    <message>
        <source>The &lt;a href=&quot;0024.html&quot;&gt;multimedia file browser&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;0024.html&quot;&gt;Мультимедиа Проводник&lt;/a&gt;</translation>
    </message>
    <message>
        <source> In this mode, you can display the multimedia files present on your hard disks and insert them using Drag &amp; Drop into the &lt;a href=&quot;0023.html&quot;&gt;timeline&lt;/a&gt; to create your slide show. </source>
        <translation>В этом режиме можно видеть мультимедийные файлы на жестких дисках и вставлять их простым перетаскиванием на &lt;a href=&quot;0023.html&quot;&gt;линию времени&lt;/a&gt; для создания слайд-шоу.</translation>
    </message>
    <message>
        <source>A small multimedia player is displayed above on the right, near the toolbar and menu, to allow you to preview the slide show. </source>
        <translation>Небольшой мультимедиа плеер отображается справа вверху, рядом с панелью инструментов и меню , позволяя вам просматривать слайд-шоу.</translation>
    </message>
    <message>
        <source>Sheet mode</source>
        <translation>Режим таблицы</translation>
    </message>
    <message>
        <source>In this mode, the main window is divided into 2 zones: </source>
        <translation>В этом режиме главное окно разделено на 2 зоны:</translation>
    </message>
    <message>
        <source> In this mode, the &lt;a href=&quot;0023.html&quot;&gt;timeline&lt;/a&gt; is displayed on several lines to show as much slides as possible of the slide show. Slides are shown as on a musical score, in the form of line and at the end of a line, we pass over to the beginning og the following line. </source>
        <translation>В этом режиме &lt;a href=&quot;0023.html&quot;&gt; линия времени&lt;/a&gt; отображается в несколько строк, чтобы показать как можно больше слайдов. Слайды показываются как на музыкальной партитуре - в виде линии, в конце которой мы переходим к началу следующей линии.</translation>
    </message>
    <message>
        <source>You can reorganize your slides by moving them using Drag &amp; Drop within the &lt;a href=&quot;0023.html&quot;&gt;timeline&lt;/a&gt;. </source>
        <translation>Вы можете упорядочивать слайды, перетаскивая их по &lt;a href=&quot;0023.html&quot;&gt;линии времени&lt;/a&gt;.</translation>
    </message>
    <message>
        <source>Preview mode</source>
        <translation>Режим предварительного просмотра</translation>
    </message>
    <message>
        <source>A big multimedia player</source>
        <translation>Большой мультимедиа плеер</translation>
    </message>
    <message>
        <source> In this mode, you can preview your slide show in big size. </source>
        <translation>В этом режиме вы можете просмотреть слайд-шоу в большом размере.</translation>
    </message>
    <message>
        <source>You can modify your slides and preview your changes in the multimedia player </source>
        <translation>Вы можете изменять ваши слайды и просматривать изменения в мультимедиа плеере</translation>
    </message>
    <message>
        <source> Switch display to file mode</source>
        <translation>Переключение экрана в режим менеджера файлов</translation>
    </message>
    <message>
        <source> Switch display to sheet mode</source>
        <translation>Переключение экрана в режим таблицы</translation>
    </message>
    <message>
        <source> Switch display to preview mode</source>
        <translation>Переключение экрана в режим предварительного просмотра</translation>
    </message>
    <message>
        <source>by using contextual menus (right click on an element)</source>
        <translation>с помощью контекстного меню (щелчок правой кнопкой мыши на элементе)</translation>
    </message>
    <message>
        <source>by double clicking on elements of the slide</source>
        <translation>двойным щелчком по элементам слайда</translation>
    </message>
    <message>
        <source>The &lt;a href=&quot;0022.html&quot;&gt;toolbar&lt;/a&gt; and menu at the top</source>
        <translation>&lt;a href=&quot;0022.html&quot;&gt; Панель инструментов&lt;/a&gt; и меню в верху</translation>
    </message>
    <message>
        <source>by using the buttons of the &lt;a href=&quot;0022.html&quot;&gt;toolbar&lt;/a&gt;</source>
        <translation>с помощью кнопок &lt;a href=&quot;0022.html&quot;&gt; панели инструментов&lt;/a&gt;</translation>
    </message>
    <message>
        <source>To change display mode, use the buttons located in the status bar, to the right, at the bottom of the main window.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The editing of slides is done:</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0022</name>
    <message>
        <source>The toolbar</source>
        <translation>Панель инструментов</translation>
    </message>
    <message>
        <source>These buttons are grouped under four tabs: File / Project / Render and Help</source>
        <translation>Эти кнопки сгруппированы в четыре вкладки: Файл / Проект / Создание видео и Помощь</translation>
    </message>
    <message>
        <source>The project management commands</source>
        <translation>Кнопки управления проектом</translation>
    </message>
    <message>
        <source>The following table summarizes the main project management commands:</source>
        <translation>В следующей таблице приведены основные команды управления проектом:</translation>
    </message>
    <message>
        <source>Toolbar button</source>
        <translation>Кнопка панели инструментов</translation>
    </message>
    <message>
        <source>Project menu command</source>
        <translation>Команда меню</translation>
    </message>
    <message>
        <source>Keyboard shortcut</source>
        <translation>Сочетание клавиш</translation>
    </message>
    <message>
        <source>Action</source>
        <translation>Действие</translation>
    </message>
    <message>
        <source>New</source>
        <translation>Новый
проект</translation>
    </message>
    <message>
        <source>CTRL+N</source>
        <translation>CTRL+N</translation>
    </message>
    <message>
        <source>(If the current project has unsaved modifications, ffDiaporama prompts you to save it.)</source>
        <translation>( Если в текущем проекте есть несохраненные изменения, ffDiaporama предложит вам сохранить его.)</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Открыть
проект</translation>
    </message>
    <message>
        <source>CTRL+O</source>
        <translation>CTRL+O</translation>
    </message>
    <message>
        <source>Allows you to open an existing project. The file selection dialog appears and allows you to select the project to be opened. </source>
        <translation>Позволяет открыть существующий проект. Появится диалоговое окно выбора файлов, которое позволит вам выбрать проект для открытия.</translation>
    </message>
    <message>
        <source>(If the current project has unsaved modifications, ffDiaporama prompts you to save it.) </source>
        <translation>( Если в текущем проекте есть несохраненные изменения, ffDiaporama предложит вам сохранить его.)</translation>
    </message>
    <message>
        <source>Remark: if the project contains files which were moved or renamed, ffDiaporama will fail to open them again. In that case, a dialog appears allowing you to select a new file.</source>
        <translation>Примечание: если проект содержит файлы, которые были перемещены или переименованы, ffDiaporama не сможет открыть их заново. В этом случае появится диалоговое окно, позволяющее вам выбрать новый файл.</translation>
    </message>
    <message>
        <source>Open recent</source>
        <translation>Открыть
недавний</translation>
    </message>
    <message>
        <source>Allows you to reopen a recently opened project. ffDiaporama stores a list of the last ten projects.</source>
        <translation>Позволяет открыть недавно открытый проект. ffDiaporama сохраняет список последних десяти проектов.</translation>
    </message>
    <message>
        <source>Save</source>
        <translation>Сохранить
проект</translation>
    </message>
    <message>
        <source>CTRL+S</source>
        <translation>CTRL+S</translation>
    </message>
    <message>
        <source>Allows you to save the current project in a project file.</source>
        <translation>Позволяет сохранить текущий проект в файл проекта.</translation>
    </message>
    <message>
        <source>If you have not saved previously the project, a file selection dialog appears and allows you to select a destination file.</source>
        <translation>Если вы раньше не сохраняли проект, появится диалоговое окно выбора файла, позволяющее указать файл назначения.</translation>
    </message>
    <message>
        <source>Save as</source>
        <translation>Сохранить
проект как</translation>
    </message>
    <message>
        <source>Project properties</source>
        <translation>Свойства
проекта</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Настройки</translation>
    </message>
    <message>
        <source>Quit</source>
        <translation>Выход</translation>
    </message>
    <message>
        <source>CTRL+W</source>
        <translation>CTRL+W</translation>
    </message>
    <message>
        <source>Allows you to exit ffDiaporama.</source>
        <translation>Позволяет выйти из ffDiaporama.</translation>
    </message>
    <message>
        <source>The slide management commands</source>
        <translation>Кнопки управления слайдами</translation>
    </message>
    <message>
        <source>The following table summarizes the slide management commands:</source>
        <translation>В следующей таблице приведены основные команды управления слайдами:</translation>
    </message>
    <message>
        <source>Edit menu command</source>
        <translation>Команда меню</translation>
    </message>
    <message>
        <source>Add title</source>
        <translation>Добавить слайд</translation>
    </message>
    <message>
        <source>Add files</source>
        <translation>Добавить файлы</translation>
    </message>
    <message>
        <source>INS</source>
        <translation>INS</translation>
    </message>
    <message>
        <source>Note: Each file selected is added to the project alphabetically by filename or in the numerical order of shots. </source>
        <translation>Примечание: каждый выбранный файл добавляется в проект в алфавитном порядке по имени файла или в числовом порядке (указывается в настройках).</translation>
    </message>
    <message>
        <source>Cameras number pictures/video sequences as they are taken; using numerical order ensures the correct chronological order of shots.</source>
        <translation>Камеры нумеруют фотографии/видео в той последовательности в которой они были сделаны; использование числового порядка гарантирует правильный хронологический порядок кадров.</translation>
    </message>
    <message>
        <source>Add project</source>
        <translation>Добавить проект</translation>
    </message>
    <message>
        <source>Cut</source>
        <translation>Вырезать</translation>
    </message>
    <message>
        <source>CTRL+X</source>
        <translation>CTRL+X</translation>
    </message>
    <message>
        <source>Copy</source>
        <translation>Копировать</translation>
    </message>
    <message>
        <source>CTRL+C</source>
        <translation>CTRL+C</translation>
    </message>
    <message>
        <source>Allows you to copy the current slide to the clipboard.</source>
        <translation>Позволяет копировать текущий слайд в буфер обмена.</translation>
    </message>
    <message>
        <source>Paste</source>
        <translation>Вставить</translation>
    </message>
    <message>
        <source>CTRL+V</source>
        <translation>CTRL+V</translation>
    </message>
    <message>
        <source>Edit</source>
        <translation>Редактировать</translation>
    </message>
    <message>
        <source>Opens a sub-menu giving access to the slide modification functions</source>
        <translation>Открывает подменю, предоставляющее доступ к функциям редактирования слайдов</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Удалить</translation>
    </message>
    <message>
        <source>DEL</source>
        <translation>DEL</translation>
    </message>
    <message>
        <source>Allows you to remove the current slide.</source>
        <translation>Позволяет удалить текущий слайд.</translation>
    </message>
    <message>
        <source>Remarks:</source>
        <translation>Примечания:</translation>
    </message>
    <message>
        <source>Inserted slides are either at the end of the project, or after the current slide, depending to the option &quot;add the new slides (at the end of the project or after the current slide) &quot; in the configuration dialog.</source>
        <translation>Вставляемые слайды добавляются либо в конец проекта, либо после текущего слайда, в зависимости от параметра «Положение объекта (в конец проекта или после текущего объекта)» в диалоговом окне Настроек программы.</translation>
    </message>
    <message>
        <source>The render commands</source>
        <translation>Кнопки создания видео</translation>
    </message>
    <message>
        <source>The following table summarizes the main render commands:</source>
        <translation>В следующей таблице приведены основные команды создания видео:</translation>
    </message>
    <message>
        <source>Render menu command</source>
        <translation>Команда меню</translation>
    </message>
    <message>
        <source>Player and Smartphone</source>
        <translation>Плеер и смартфон</translation>
    </message>
    <message>
        <source>Multimedia system</source>
        <translation>Мультимедиа система</translation>
    </message>
    <message>
        <source>For the WEB</source>
        <translation>Интернет</translation>
    </message>
    <message>
        <source>Lossless</source>
        <translation>Сжатие без потерь</translation>
    </message>
    <message>
        <source>Advanced</source>
        <translation>Дополнительно</translation>
    </message>
    <message>
        <source>Remark:</source>
        <translation>Примечание:</translation>
    </message>
    <message>
        <source>The lossless format is readable only by few equipment and generates files of enormous size. Consequently, it is useful only to generate parts of a project which will then be joined into a final project.</source>
        <translation>Формат &quot;без потери качества&quot; предназначен для воспроизведения только на некоторых типах оборудования и создает файлы огромных размеров. Следовательно, он полезен только для создания отдельных частей проекта, которые потом будут добавлены к финальному проекту и снова подвергнутся кодированию.</translation>
    </message>
    <message>
        <source>The help commands</source>
        <translation>Кнопки помощи</translation>
    </message>
    <message>
        <source>The following table summarizes the main help commands:</source>
        <translation>В следующей таблице приведены основные команды помощи:</translation>
    </message>
    <message>
        <source>Help menu command</source>
        <translation>Команда меню</translation>
    </message>
    <message>
        <source>About</source>
        <translation>О программе</translation>
    </message>
    <message>
        <source>Opens the about dialog. This displays information about the release, license and system information</source>
        <translation>Открываетдиалоговое окно &quot;О программе&quot;. Оно содержит информацию о версии, лицензии и системную информацию</translation>
    </message>
    <message>
        <source>Support and help (F1)</source>
        <translation>Справка (F1)</translation>
    </message>
    <message>
        <source>Opens the ffDiaporama WIKI index</source>
        <translation>Открывает содержание ffDiaporama WIKI</translation>
    </message>
    <message>
        <source>What&apos;s new</source>
        <translation>Что нового</translation>
    </message>
    <message>
        <source>Opens the Blog/News page from the ffDiaporama website</source>
        <translation>Открывает страницу Новости сайта ffDiaporama</translation>
    </message>
    <message>
        <source>Allows you to create a new blank project. The &lt;a href=&quot;0108.html&quot;&gt;project properties dialog&lt;/a&gt; appears and allows you to setup the new project. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Opens a sub-menu giving you access to the following functions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Save as: Allows you to save the current project in a different file. The &lt;a href=&quot;0109.html&quot;&gt;file selection dialog&lt;/a&gt; appears and allows you to select a destination file.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Export project: Export current project and all used files in a new folder. The &lt;a href=&quot;0107.html&quot;&gt;export project dialog&lt;/a&gt; appears and allows you to setup the export.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to open the &lt;a href=&quot;0108.html&quot;&gt;project properties dialog&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to define applications settings. The &lt;a href=&quot;0101.html&quot;&gt;application settings dialog&lt;/a&gt; appears.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Add empty slide: Allows you to add an empty slide : the new empty slide is appended to the project and is displayed in the &lt;a href=&quot;0023.html&quot;&gt;timeline&lt;/a&gt;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Add a predefined title slide: Allows you to add a slide based on a model. The &lt;a href=&quot;0103.html&quot;&gt;predefined title slide properties dialog&lt;/a&gt; appears.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to add photo or video files : A &lt;a href=&quot;0109.html&quot;&gt;file selection dialog&lt;/a&gt; appears and allows you to select one or more files. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to add the slides of an existing project to the end the current project: a &lt;a href=&quot;0109.html&quot;&gt;file selection dialog&lt;/a&gt; appears and allows you to select a project file. Slides, slide settings, music and background of the project are added to the current project.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to cut the current slide (remove it from the &lt;a href=&quot;0023.html&quot;&gt;timeline&lt;/a&gt; and put it in the clipboard).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to paste the slide currently in the clipboard into the &lt;a href=&quot;0023.html&quot;&gt;timeline&lt;/a&gt;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Open the &lt;a href=&quot;0104.html&quot;&gt;background properties dialog&lt;/a&gt; (Equivalent to a double click on the background track)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>F6 </source>
        <translation type="unfinished">F6 </translation>
    </message>
    <message>
        <source>Depending on the slide type, open the &lt;a href=&quot;0119.html&quot;&gt;slide properties dialog&lt;/a&gt; if it&apos;s a standard slide or open the &lt;a href=&quot;0103.html&quot;&gt;predefined title slide properties dialog&lt;/a&gt; if it&apos;s a predefined title slide. (Equivalent to a double click on the mounting track)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>F7 </source>
        <translation type="unfinished">F7 </translation>
    </message>
    <message>
        <source>Open the &lt;a href=&quot;0114.html&quot;&gt;music properties dialog&lt;/a&gt; (Equivalent to a double click on the music track)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>F8 </source>
        <translation type="unfinished">F8 </translation>
    </message>
    <message>
        <source>Open the &lt;a href=&quot;0122.html&quot;&gt;transition properties dialog&lt;/a&gt; (Equivalent to a double click on the transition)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>It is possible to add slides by Drag and Drop directly using the file browser. Drag photos or videos onto the &lt;a href=&quot;0023.html&quot;&gt;timeline&lt;/a&gt; and slides are created at the point you release the files.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to render the video for selected equipment from the device database. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The &lt;a href=&quot;0115.html&quot;&gt;render movie dialog&lt;/a&gt; from database appears.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to render the video in a lossless format. Lossless format is not destructive, so it is possible to render several times the same video without losing in quality. The &lt;a href=&quot;0115.html&quot;&gt;render movie dialog&lt;/a&gt; appears.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to render the video using the &lt;a href=&quot;0115.html&quot;&gt;Advanced render movie dialog&lt;/a&gt;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Export Soundtrack</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to render only the sound track of the project. The &lt;a href=&quot;0115.html&quot;&gt;render movie dialog&lt;/a&gt; appears.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0023</name>
    <message>
        <source>The timeline</source>
        <translation>Линия времени (таймлайн)</translation>
    </message>
    <message>
        <source>The timeline lists the slides which compose the project. The slides are listed from left to right.</source>
        <translation>На линии времени приведены слайды, составляющие проект. Они воспроизводятся слева направо.</translation>
    </message>
    <message>
        <source>Each slide is represented by the 3 tracks:</source>
        <translation>Каждый слайд представлен 3 треками:</translation>
    </message>
    <message>
        <source>The background track:</source>
        <translation>Трек фона:</translation>
    </message>
    <message>
        <source>An icon corresponding to the transition selected to pass from the previous background to this slide (if a new background is defined). .</source>
        <translation>значок перехода, свидетельствующий о смене фона предыдущего слайда на фон этого слайда (если определен новый фон).</translation>
    </message>
    <message>
        <source>A thumbnail representing the background associated with this slide.</source>
        <translation>миниатюра фона этого слайда.</translation>
    </message>
    <message>
        <source>The slide number.</source>
        <translation>номер слайда.</translation>
    </message>
    <message>
        <source> Possibly this icon may appear at the upper left if the slide is used as chapter start</source>
        <translation>этот значок может появиться  в левом верхнем углу, если слайд является началом новой главы</translation>
    </message>
    <message>
        <source>The mounting track:</source>
        <translation>Трек монтажа:</translation>
    </message>
    <message>
        <source>An icon corresponding to the transition selected to pass from the previous slide to this slide, under which is indicated the duration of the transition in seconds.</source>
        <translation>значок  перехода от предыдущего слайда к этому слайду, под которым указывается продолжительность перехода в секундах.</translation>
    </message>
    <message>
        <source>A thumbnail in the form of a static image for slides consisting of title or photo, or in the form of a video (under which is placed a soundtrack).</source>
        <translation>миниатюра либо в виде изображения для слайдов, состоящих из заголовка или фото, либо в виде пленки под которой помещается саундтрек для видео.</translation>
    </message>
    <message>
        <source> Possibly this icon may appear at the upper left if the slide is composed of several blocks.</source>
        <translation>этот значок может появиться в верхнем левом углу, если слайд состоит из нескольких блоков.</translation>
    </message>
    <message>
        <source> Possibly this icon may appear at the upper right if the slide is animated (that is consisting of more than one shot).</source>
        <translation>этот значок может появиться в верхнем правом углу, если слайд анимирован (состоит из более чем одного кадра).</translation>
    </message>
    <message>
        <source> Possibly this icon may appear at the bottom right if the slide contains at least an image for which you defined a transformation.</source>
        <translation>этот значок может появиться в нижнем правом углу, если слайд содержит по крайней мере изображение для которого определено преобразование.</translation>
    </message>
    <message>
        <source> Possibly this icon may appear at the bottom left if the slide is an automatic predefined slide.</source>
        <translation>этот значок может появиться в нижнем левом углу, если слайд создан на основе шаблона.</translation>
    </message>
    <message>
        <source> Possibly this icon may appear at the bottom right if the predefined slide is a project title slide.</source>
        <translation>этот значок может появиться в нижнем правом углу, если слайд является шаблоном названий проекта.</translation>
    </message>
    <message>
        <source> Possibly this icon may appear at the bottom right if the predefined slide is a chapter title slide.</source>
        <translation>этот значок может появиться в нижнем правом углу, если слайд является шаблоном названий глав.</translation>
    </message>
    <message>
        <source> Possibly this icon may appear at the bottom right if the predefined slide is a generic title slide.</source>
        <translation>этот значок может появиться в нижнем правом углу, если слайд является универсальным шаблоном названий.</translation>
    </message>
    <message>
        <source>Remark: The thumbnail represents the first image on the slide.</source>
        <translation>Примечание: миниатюра представляет собой первое из изображений слайда.</translation>
    </message>
    <message>
        <source>The music track:</source>
        <translation>Трек музыки:</translation>
    </message>
    <message>
        <source>Every play-list is shown by a change of color.</source>
        <translation>Каждый плейлист выделен чередующимся цветом.</translation>
    </message>
    <message>
        <source>The track is more or less filled according to the sound volume selected, displaying the variation in volume.</source>
        <translation>Уровень громкости звука пропорционален толщине (высоте) музыкального трека.</translation>
    </message>
    <message>
        <source>The musical transitions (fade in/out) are shown as crossings.</source>
        <translation>Переход звука (затухание / нарастание) отображается в виде пересечения аудиодорожек.</translation>
    </message>
    <message>
        <source>Remark: Possible pauses are symbolized by a change in the volume to zero and the display of this icon: </source>
        <translation>Примечание: постановка звука на паузу показывается изменением громкости на ноль и отображением этого значка: </translation>
    </message>
    <message>
        <source>To modify an element:</source>
        <translation>Изменение элементов:</translation>
    </message>
    <message>
        <source>You can also call a contextual menu by doing a right click ahead the thumbnail or click on the Edit button. This contextual menu allows you to select an action.</source>
        <translation>Также вы можете вызвать контекстное меню, щелкнув правой кнопкой мыши на миниатюре или нажав кнопку Редактировать. Контекстное меню позволяет вам выбрать нужное действие.</translation>
    </message>
    <message>
        <source>To modify several elements at one time:</source>
        <translation>Одновременное изменение нескольких элементов:</translation>
    </message>
    <message>
        <source>You can select several slides by using the mouse and the CTRL and SHIFT:</source>
        <translation>Вы можете выделить несколько слайдов мышью, держа зажатой клавишу CTRL или SHIFT:</translation>
    </message>
    <message>
        <source>CTRL + click on a slide: add or removes the slide of the selection</source>
        <translation>CTRL + щелчок на слайде: добавление или удаление слайда из выделения</translation>
    </message>
    <message>
        <source>SHIFT + click on a slide: add all the slides understood between the last one selected and the slide which you have just clicked</source>
        <translation>SHIFT + щелчок на слайде: добавление всех слайдов, расположенных между выделенным сейчас слайдом и слайдом на котором вы щелкнули</translation>
    </message>
    <message>
        <source>Then, by making a right click on one of the slides of the selection, a contextual menu appears.</source>
        <translation>Затем щелчком правой кнопкой мыши на одном из выделенных слайдов вызывается контекстное меню.</translation>
    </message>
    <message>
        <source>This menu suggests various actions making on all the slides of the selection such as:</source>
        <translation>Это меню предлагает различные действия для всех выделенных слайдов. Например:</translation>
    </message>
    <message>
        <source>Copy/Cut/Paste/Remove all slide of the selection</source>
        <translation>Копироват/вставить/удалить все выделенные слайды</translation>
    </message>
    <message>
        <source>Modify the duration of the 1st shot for all the slides of the selection</source>
        <translation>Изменить длительность первого кадра для всех выделенных слайдов</translation>
    </message>
    <message>
        <source>Modify the transition for all the slides of the selection</source>
        <translation>Изменить переходы для всех выделенных слайдов</translation>
    </message>
    <message>
        <source>Modify the music for all the slides of the selection</source>
        <translation>Изменить музыку для всех выделенных слайдов</translation>
    </message>
    <message>
        <source>etc...</source>
        <translation>и др.</translation>
    </message>
    <message>
        <source>To reorder the slide in the timeline:</source>
        <translation>Изменение порядка слайдов на линии времени:</translation>
    </message>
    <message>
        <source>Simply drag the slide to a new position with the mouse (drag &amp; drop)</source>
        <translation>Просто перетащите слайд мышью на новую позицию</translation>
    </message>
    <message>
        <source>To modify the display of the timeline:</source>
        <translation>Изменение отображения линии времени:</translation>
    </message>
    <message>
        <source>The timeline settings commands are in the status bar, to the right, at the bottom of the main window.</source>
        <translation>Команды настройки линии времени расположены в строке состояния, справа, в нижней части главного окна.</translation>
    </message>
    <message>
        <source> Increases the size of the thumbnails but reduces the number of thumbnails shown in the timeline.</source>
        <translation>Увеличивает размер, но уменьшает количество показываемых на линии времени миниатюр.</translation>
    </message>
    <message>
        <source> Reduces the size of the thumbnails and increases the number of thumbnails shown in the timeline.</source>
        <translation>Уменьшает размер, но увеличивает количество показываемых на линии времени миниатюр.</translation>
    </message>
    <message>
        <source>To modify the background, double-click the thumbnail of the background. This action opens the &lt;a href=&quot;0104.html&quot;&gt;Define background&lt;/a&gt; dialog.</source>
        <translation>Чтобы изменить фон, дважды щелкните на миниатюре фона. Это действие открывает диалоговое окно &lt;a href=&quot;0104.html&quot;&gt; Свойства фона&lt;/a&gt;.</translation>
    </message>
    <message>
        <source>To modify the entering slide transition, double-click the slide transition. This action opens the &lt;a href=&quot;0122.html&quot;&gt;Define the entering slides transitions&lt;/a&gt; dialog.</source>
        <translation>Чтобы изменить переход, дважды щелкните на преходе слайда. Это действие открывает диалоговое окно &lt;a href=&quot;0122.html&quot;&gt; Параметры перехода&lt;/a&gt;.</translation>
    </message>
    <message>
        <source>To modify the music associated with the slide, double-click the zone corresponding to the slide musical track. This action opens the &lt;a href=&quot;0114.html&quot;&gt;Define the music track&lt;/a&gt; dialog.</source>
        <translation>Чтобы изменить музыку, связанную со слайдом, дважды щелкните в зоне, соответствующей музыкальному треку слайда. Это действие открывает диалоговое окно &lt;a href=&quot;0114.html&quot;&gt;Параметры музыки&lt;/a&gt;.</translation>
    </message>
    <message>
        <source>To modify the slide (framing, filter, animation, etc.) double-click the thumbnail. This action opens the &lt;a href=&quot;0119.html&quot;&gt;slides properties dialog&lt;/a&gt; if it&apos;s a standard slide or open the &lt;a href=&quot;0103.html&quot;&gt;predefined title slide properties dialog&lt;/a&gt; if it&apos;s a predefined title slide.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0024</name>
    <message>
        <source>The multimedia file browser</source>
        <translation>Мультимедиа Проводник</translation>
    </message>
    <message>
        <source>ffDiaporama offers a multimedia file browser which is shown in the main window when you select the file browser mode in the main window. It is designed for quick access to the multimedia files on your computer.</source>
        <translation>ffDiaporama предлагает мультимедийный менеджер файлов, который появляется в главном окне, когда вы переключаетесь в режим менеджера файлов. Он предназначен для быстрого доступа к мультимедийным файлам на вашем компьютере.</translation>
    </message>
    <message>
        <source>The main advantages are, that you can view the multimedia information of specific files even without or before adding them to your project. For instance, you can filter the view to obtain information of only photos, video files or music files.</source>
        <translation>Основное его преимущество состоит в том, что вы можете просмотреть мкльтимедиа информацию о конкретных файлах перед их добавлением в проект (без добавления). Например, можно настроить отображение только фотографий, видео или музыкальных файлов.</translation>
    </message>
    <message>
        <source>And moreover, you can select one or more multimedia files and add them to your project with drag &amp; drop.</source>
        <translation>Кроме того, можно выбрать один или несколько мультимедиа файлов и добавить их в проект с помощью перетаскивания.</translation>
    </message>
    <message>
        <source>The main purpose of the file browser is therefore the viewing and use of (multimedia) files. Some file management tasks are integrated for your convenience, i.e. creating folders deleting files or folders and renaming files or folders.</source>
        <translation>Основная цель менеджера файлов - просмотр и использование (мультимедиа) файлов. Для вашего удобства встроены некоторые задачи управления файлами: создание папок, удаление файлов или папок и переименования файлов или папок.</translation>
    </message>
    <message>
        <source>Remark:</source>
        <translation>Примечание:</translation>
    </message>
    <message>
        <source>Files can be presented differently than by the system file browser (presence of filters showing only some files - management of the file/icon pairs - etc.)</source>
        <translation>Файлы могут быть представлены иначе, чем в системном менеджере файлов (наличие фильтров отображения только некоторых файлов, управление парами файл - значок,  и др.)</translation>
    </message>
    <message>
        <source>The multimedia file browser follows the display rules used by the majority of multimedia systems (gateway, multimedia hard drive, etc.): </source>
        <translation>Мультимедиа Проводник использует следующие правила отображения основных мультимедиа систем (шлюз, мультимедийный жесткий диск, и др.):</translation>
    </message>
    <message>
        <source>If a drive or a folder contains a folder.jpg file, then this image file is used as icon for the drive or the folder</source>
        <translation>Если диск или папка содержат файл folder.jpg, то это изображение используется как значок для диска или папки</translation>
    </message>
    <message>
        <source>If a video or music file is accompanied with an image file with the .jpg extension carrying the same name, then this image file is used as icon for this file (example: holidays.mkv and holidays.jpg in the same folder</source>
        <translation>Если видео или музыкальный файл сопровождаются изображением с расширением .jpg с таким же именем, то это изображение используется как значок для этого файла (например: файлы holidays.mkv и holidays.jpg в одной папке)</translation>
    </message>
    <message>
        <source>The ffDiaporama multimedia file browser is not designed to manage files: so you cannot make copies of files or move files.</source>
        <translation>Мультимедиа Проводник ffDiaporama не предназначен для работы с файлами : вы не cможете копировать и перемещать файлы.</translation>
    </message>
    <message>
        <source> Folder tree</source>
        <translation>Дерево папок</translation>
    </message>
    <message>
        <source>The folder tree shows the drives and folders of your system. Under every drive the folders of that drive are listed, then under every folder the subfolders, if present and so on.</source>
        <translation>Дерево папок показывает диски и папки вашей системы. Под каждым диском перечислены папки этого диска, а под каждой папкой - вложенные в нее папки, если они есть,  и так далее.</translation>
    </message>
    <message>
        <source>You can:</source>
        <translation>Вы можете:</translation>
    </message>
    <message>
        <source>Click a drive or a folder to select it</source>
        <translation>Щелкнуть на диске или папке для их выделения</translation>
    </message>
    <message>
        <source>Click the triangle preceding the drive name or folder name to display or hide the list of subfolders</source>
        <translation>Нажать на треугольник возле имени диска или папки для отображения или скрытия списка вложенных папок</translation>
    </message>
    <message>
        <source>Make a right click on a drive or a folder to call the contextual menu on this drive or folder. The contextual menu proposes then the following options:</source>
        <translation>Щелкнуть правой кнопкой мыши на диске или папке для вызова контекстного меню этого диска или папки. В контекстном меню доступны следующие опции:</translation>
    </message>
    <message>
        <source>&lt;B&gt;Refresh all&lt;/B&gt; to update the whole tree (as so if you have to insert a CD-ROM, a DVD or an USB drive, this one is shown in the tree)</source>
        <translation>&lt;B&gt;Обновить все&lt;/B&gt;, чтобы обновить все дерево (если вы вставите CD-ROM, DVD или USB-накопитель,то после этого он отобразится в дереве)</translation>
    </message>
    <message>
        <source>&lt;B&gt;Refresh from here&lt;/B&gt; to update the tree from this folder (update only what is under this folder)</source>
        <translation>&lt;B&gt;Обновить выделенное&lt;/B&gt; для обновления дерева этой папки (обновить только то, что находится в этой папке)</translation>
    </message>
    <message>
        <source>Create a new folder</source>
        <translation>Создать новую папку</translation>
    </message>
    <message>
        <source>Remove a folder</source>
        <translation>Удалить папку</translation>
    </message>
    <message>
        <source>Rename a folder</source>
        <translation>Переименовать папку</translation>
    </message>
    <message>
        <source>The current folder view lists files and subfolders present in the current folder.</source>
        <translation>В текущей папке показывается список файлов и подкаталогов, содержащихся в ней.</translation>
    </message>
    <message>
        <source>Files are shown:</source>
        <translation>Файлы показываются:</translation>
    </message>
    <message>
        <source>Either as a table with numerous information if you selected the details view in the settings menu of the browser</source>
        <translation> как таблица с подробной информацией, если вы выбрали вид &quot;Подробно&quot;  в меню настроек Проводника</translation>
    </message>
    <message>
        <source>Or as image if you selected the icons view in the settings menu of the browser</source>
        <translation>как изображения, если вы выбрали вид &quot;Значки&quot; в меню настроек Проводника</translation>
    </message>
    <message>
        <source>Files are shown according to the filter of file selection in the settings menu of the browser</source>
        <translation>Файлы отображаются на основе фильтра, выбранного в меню настроек Проводника</translation>
    </message>
    <message>
        <source>Select a file or a folder by clicking it</source>
        <translation>выбрать файл или папку, нажав на его</translation>
    </message>
    <message>
        <source>Select several files or folders by using SHIFT and CTRL keys</source>
        <translation>выбрать несколько файлов или папок с помощью клавиш SHIFT или CTRL</translation>
    </message>
    <message>
        <source>CTRL+Click on a file: Add to the current selection or remove from the current selection</source>
        <translation>CTRL + щелчок на файле: добавляют его к выделению или удаляют его из выделения</translation>
    </message>
    <message>
        <source>SHIFT+Click on a file: Select all the files between the last selected and the one which you have just clicked</source>
        <translation>SHIFT + щелчок на файле: выделяют все файлы, расположенных между выделенным сейчас файлом и файлом на котором вы щелкнули</translation>
    </message>
    <message>
        <source>Double click a file or a folder:</source>
        <translation>двойной щелчок на файле или папке:</translation>
    </message>
    <message>
        <source>If it&apos;s a folder: select it as the current folder and opens it</source>
        <translation>если это папка: выбрать ее в качестве текущей папки и открыть</translation>
    </message>
    <message>
        <source>If it&apos;s a file: open the program that is by default associated with the file type in the system. For example, if you have associated GIMP with .jpg files in the system file browser, then a double click on a .jpg file will open the file with GIMP. (Remark: A double click on a ffDiaporama project file, opens the ffDiaporama project)</source>
        <translation>если это файл: открывает программу, которая ассоциирована с этим типом файла в системе. Например, если GIMP ассоциирован с .jpg файлами в системном менеджере файлов, то двойной щелчок на .jpg файле откроет его в GIMP. (Примечание: двойной щелчок на файле проекта ffDiaporama открывает проект в ffDiaporama)</translation>
    </message>
    <message>
        <source>Perform a right click on one of the files or folders of the selection to open the contextual menu. According to the elements that are present in the selection, the menu will propose different actions</source>
        <translation>выполнить щелчок правой кнопкой мыши на выделенных файлах или папках для вызова контекстного меню. В зависимости от типа элементов, которые выделены, в меню будут доступны различные действия</translation>
    </message>
    <message>
        <source>You can also use Drag &amp; Drop to move selected files towards the timeline to:</source>
        <translation>Также можно перетащить выделенные файлы на линию времени, чтобы:</translation>
    </message>
    <message>
        <source>Add them to the project, if they are images, videos or ffDiaporama project files</source>
        <translation>добавить их в проект, если они являются изображениями, видео или файлами проекта ffDiaporama</translation>
    </message>
    <message>
        <source>Use them as playlist, if they are audio files</source>
        <translation>использовать их в качестве плейлиста, если они являются аудиофайлами</translation>
    </message>
    <message>
        <source>Status bar to the current selection</source>
        <translation>Строка состояния текущего выделения</translation>
    </message>
    <message>
        <source>The status bar to the current selection shows information about one or several selected files.</source>
        <translation>Строка состояния текущего выделения показывает информацию об одном или нескольких выделенных файлах.</translation>
    </message>
    <message>
        <source>For a single file selection, the following information is shown:</source>
        <translation>Если выделен один файл, отображается следующая информация:</translation>
    </message>
    <message>
        <source>Image of the file</source>
        <translation>Миниатюра файла</translation>
    </message>
    <message>
        <source>Filename (Filesize)</source>
        <translation>Имя файла (размер файла)</translation>
    </message>
    <message>
        <source>According to the file type: </source>
        <translation>В зависимости от типа файла:</translation>
    </message>
    <message>
        <source>Image: Information about the size of the image / copyright / camera / orientation </source>
        <translation>Изображение: информация о размере / авторском праве / камере / ориентации</translation>
    </message>
    <message>
        <source>Video: Information about the format of the images / the video track / the audio track / duration </source>
        <translation>Видео: информация о формате изображения / видео дорожке / аудио дорожке / продолжительности</translation>
    </message>
    <message>
        <source>Music: Information about the audio format / duration </source>
        <translation>Музыка: информация об аудио формате / продолжительности</translation>
    </message>
    <message>
        <source>ffDiaporama project: Version of ffDiaporama / Format / Number of slide and chapter / duration of the project </source>
        <translation>Проект ffDiaporama : версия ffDiaporama / формат / количество слайдов и глав / продолжительность проекта</translation>
    </message>
    <message>
        <source>Image : Information about the picture shooting (Aperture and Focal / Lens / With or without flash) </source>
        <translation>Изображение: информация о режиме съемки ( диафрагма и фокусное расстояние / объектив / наличие вспышки )</translation>
    </message>
    <message>
        <source>Video: TAG contents (Title / Artist / Album / Year) </source>
        <translation>Видео: содержимое тегов (название / исполнитель / альбом / год)</translation>
    </message>
    <message>
        <source>Music: TAG contents (Title / Artist / Album / Year) </source>
        <translation>Музыка: содержимое тегов (название / исполнитель / альбом / год)</translation>
    </message>
    <message>
        <source>ffDiaporama project: TAG contents (Title / Artist / Album / Year) </source>
        <translation>Проект ffDiaporama: содержимое тегов (название / исполнитель / альбом / год)</translation>
    </message>
    <message>
        <source>For a multiple files selection, the following information is shown:</source>
        <translation>Если выделено несколько файлов, отображается следующая информация:</translation>
    </message>
    <message>
        <source>Icon of the file type</source>
        <translation>Значок типа файла</translation>
    </message>
    <message>
        <source>Number of files and type of files</source>
        <translation>Количество файлов и тип файлов</translation>
    </message>
    <message>
        <source>Durations accumulated by files</source>
        <translation>Общая продолжительность файлов</translation>
    </message>
    <message>
        <source>Size accumulated by files</source>
        <translation>Общий размер файлов</translation>
    </message>
    <message>
        <source>Status bar of the current folder</source>
        <translation>Строка состояния текущей папки</translation>
    </message>
    <message>
        <source>The status bar of the current folder shows the following information about the current folder:</source>
        <translation>Строка состояния текущей папки показывает следующую информацию о текущей папке:</translation>
    </message>
    <message>
        <source> Number of files</source>
        <translation>Количество файлов</translation>
    </message>
    <message>
        <source> Number of sub-folders</source>
        <translation>Количество подкаталогов</translation>
    </message>
    <message>
        <source> Total size of shown files</source>
        <translation>Общий размер отображаемых файлов</translation>
    </message>
    <message>
        <source> Total duration of shown files</source>
        <translation>Общая продолжительность отображаемых файлов</translation>
    </message>
    <message>
        <source>Space occupied on the disk / Total space of the disk</source>
        <translation>Занято на диске / общий размер диска</translation>
    </message>
    <message>
        <source> Favorites menu</source>
        <translation>Меню Избранное</translation>
    </message>
    <message>
        <source>The list of the favourite folders can be managed in the favorites menu.</source>
        <translation>Списком Избранных папко можно управлять из меню избранного.</translation>
    </message>
    <message>
        <source>First line of the menu: </source>
        <translation>Первая строка меню: </translation>
    </message>
    <message>
        <source>&lt;U&gt;&lt;B&gt; Add to favorite&lt;/B&gt;&lt;/U&gt; adds the current folder to the list. Then an edit box allows you to give a name for this favorite</source>
        <translation>&lt;U&gt;&lt;B&gt;Добавить в избранное&lt;/B&gt;&lt;/U&gt; добавляет текущую папку в список. Затем вам потребуется указать ее имя в избранном</translation>
    </message>
    <message>
        <source>Between the first and the last line: </source>
        <translation>Между первой и последней строкой: </translation>
    </message>
    <message>
        <source> &lt;U&gt;&lt;B&gt;The list of favorites folders&lt;/B&gt;&lt;/U&gt; is displayed. Select one of the favorite folders to access it directly</source>
        <translation>отображается &lt;U&gt;&lt;B&gt;Список Избранных папок&lt;/B&gt;&lt;/U&gt;. Выберите одну из Избранных папок для непосредственного перехода в нее</translation>
    </message>
    <message>
        <source>Last line of the menu: </source>
        <translation>Последняя строка меню: </translation>
    </message>
    <message>
        <source>&lt;U&gt;&lt;B&gt; Manage favorite&lt;/B&gt;&lt;/U&gt; allows to rename or to delete entries of the list. A dialogbox for the modification of the list is displayed</source>
        <translation>&lt;U&gt;&lt;B&gt;Управление избранным&lt;/B&gt;&lt;/U&gt; позволяет переименовать или удалить Избранное. Появится диалоговое окно для изменения списка</translation>
    </message>
    <message>
        <source> Settings menu</source>
        <translation>Меню настроек</translation>
    </message>
    <message>
        <source>The settings menu allows to modify the display of the current folder:</source>
        <translation>Меню настроек позволяет изменить отображение текущей папки:</translation>
    </message>
    <message>
        <source>Settings of the display mode</source>
        <translation>Настройки режима отображения</translation>
    </message>
    <message>
        <source>Details view</source>
        <translation>Подробно</translation>
    </message>
    <message>
        <source>Switch the display of the current folder to detail mode. In this mode, the present elements in the folder are shown in a table with numerous information</source>
        <translation>Переключает отображение текущей папки в подробный режим. В этом режиме имеющиеся в папке элементы представлены в виде таблице с многочисленной информацией</translation>
    </message>
    <message>
        <source>Icon view</source>
        <translation>Значки</translation>
    </message>
    <message>
        <source>Switch the display of the current folder to icon mode. In this mode, the present elements in the folder are shown as images</source>
        <translation>Переключает отображение текущей папки в режим значков. В этом режиме имеющиеся в папке элементы представлены в виде изображений</translation>
    </message>
    <message>
        <source>Settings of the filter on files</source>
        <translation>Настройки фильтрации файлов</translation>
    </message>
    <message>
        <source>All files</source>
        <translation>Все файлы</translation>
    </message>
    <message>
        <source>Show all the present elements in the current folder</source>
        <translation>Показывает все имеющиеся в текущей папке элементы</translation>
    </message>
    <message>
        <source>Managed files</source>
        <translation>Поддерживаемые файлы</translation>
    </message>
    <message>
        <source>Limit the display to the usable elements of the current folder:</source>
        <translation>Ограничивает отображение имеющихся в текущей папке элементов:</translation>
    </message>
    <message>
        <source>Image, video and audio files of which the format is recognized by ffDiaporama</source>
        <translation>изображениями, видео и аудио файлами, поддерживаемых ffDiaporama форматов</translation>
    </message>
    <message>
        <source>Subfolders</source>
        <translation>подпапками</translation>
    </message>
    <message>
        <source>Image files</source>
        <translation>Изображения</translation>
    </message>
    <message>
        <source>Limit the display of the current folder to the image files of which the format is recognized by ffDiaporama in the present subfolders</source>
        <translation>Ограничивает отображение текущей папки только изображениями, поддерживаемых ffDiaporama форматов</translation>
    </message>
    <message>
        <source>Video files</source>
        <translation>Видео файлы</translation>
    </message>
    <message>
        <source>Limit the display of the current folder to the video files of which the format is recognized by ffDiaporama in the present subfolders</source>
        <translation>Ограничивает отображение текущей папки только видео файлами, поддерживаемых ffDiaporama форматов</translation>
    </message>
    <message>
        <source>Audio files</source>
        <translation>Аудио файлы</translation>
    </message>
    <message>
        <source>Limit the display of the current folder to the audio files of which the format is recognized by ffDiaporama inthe present subfolders</source>
        <translation>Ограничивает отображение текущей папки только аудио файлами, поддерживаемых ffDiaporama форматов</translation>
    </message>
    <message>
        <source>ffDiaporama projects</source>
        <translation>Файлы проектов ffDiaporama</translation>
    </message>
    <message>
        <source>Limit the display of the current folder to the ffDiaporama project files in the present subfolders</source>
        <translation>Ограничивает отображение текущей папки только файлами проектов ffDiaporama</translation>
    </message>
    <message>
        <source>Additional settings</source>
        <translation>Дополнительные настройки</translation>
    </message>
    <message>
        <source>Display hidden files and folders</source>
        <translation>Показать скрытые файлы и папки</translation>
    </message>
    <message>
        <source>Show or hide the hidden files and folders. </source>
        <translation>Показывает или скрывает скрытые файлы и папки. </translation>
    </message>
    <message>
        <source>Under Windows, hidden files and folders have the &lt;B&gt;Hide&lt;/B&gt; file attribute activated. </source>
        <translation>В Windows скрытые файлы и папки имеют атрибут &lt;B&gt;Скрытый&lt;/B&gt; .</translation>
    </message>
    <message>
        <source>Under Linux, hidden files and folders have a name beginning with a dot &quot;.&quot; </source>
        <translation>В Linux названия скрытых файлов и папок начинаются точкой &quot;.&quot; </translation>
    </message>
    <message>
        <source>This option accumulates both versions for both systems. So, even under Windows, files and folders with a name beginning with a &quot;.&quot; are considered as hidden files or folders. </source>
        <translation>Этот параметр поддерживает оба вида скрытия. Так, даже в Windows, файлы и папки имя которых начинается с &quot;.&quot;, рассматриваются как скрытые файлы или папки. </translation>
    </message>
    <message>
        <source>Remark: </source>
        <translation>Примечание: </translation>
    </message>
    <message>
        <source> or </source>
        <translation> или </translation>
    </message>
    <message>
        <source> One of this icon is displayed in front of the name of the option as this one is activated or not.</source>
        <translation>Один из этих значков отображается перед названием параметра в зависимости от того активирован он или нет.</translation>
    </message>
    <message>
        <source>Hide filename</source>
        <translation>Скрыть имена файлов</translation>
    </message>
    <message>
        <source>This option is available only for the icon view mode. If this option is activated, the file names will not be shown under the file images. It allows to increase the number of images shown in the view of the current directory. </source>
        <translation>Этот параметр доступен только для режима Значки. Если эта опция активирована, имена файлов не будут отображаться. Это позволяет увеличить количество отображаемых в данном режиме элементов. </translation>
    </message>
</context>
<context>
    <name>0030</name>
    <message>
        <source>Rendering videos</source>
        <translation>Создание видео</translation>
    </message>
    <message>
        <source>Smartphones and tablets</source>
        <translation>Смартфоны и планшеты</translation>
    </message>
    <message>
        <source>Portable devices</source>
        <translation>Портативные устройства</translation>
    </message>
    <message>
        <source>Multimedia system (Home cinema)</source>
        <translation>Мультимедиа система (домашний кинотеатр)</translation>
    </message>
    <message>
        <source>For the WEB</source>
        <translation>Интернет</translation>
    </message>
    <message>
        <source>Video codec</source>
        <translation>Видео кодек</translation>
    </message>
    <message>
        <source>Audio codec</source>
        <translation>Аудио кодек</translation>
    </message>
    <message>
        <source>Containers</source>
        <translation>Контейнеры</translation>
    </message>
    <message>
        <source>Video standards and resolutions</source>
        <translation>Видео стандарты и разрешения</translation>
    </message>
    <message>
        <source>File attachments (Thumbnails, nfo, etc...)</source>
        <translation>Присоединяемые файлы (обложка, nfo, и др.)</translation>
    </message>
    <message>
        <source>ffDiaporama WIKI - Rendering videos</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This section contains information about generating video with ffDiaporama. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Index</source>
        <translation type="unfinished">Содержание</translation>
    </message>
</context>
<context>
    <name>0031</name>
    <message>
        <source>Rendering videos</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A digital video consists of a computer file called the &lt;a href=&quot;0038.html&quot;&gt;container&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>In this file we find:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>One or more video tracks. Each track is built using a &lt;a href=&quot;0036.html&quot;&gt;Video Codec&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>One or more audio tracks. Each track is built using an &lt;a href=&quot;0037.html&quot;&gt;Audio Codec&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Possibly, one or more subtitles. Each subtitle is built in a subtitle format.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Possibly, a definition of tags and chapters.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Limitations :</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>ffDiaporama does not manage subtitles.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>ffDiaporama generates files that include a single video track and a single audio track.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Unlike music files, which are widely portable from one device to another, videos require a certain amount of juggling between various formats to adapt to the equipment used to play them.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A video rendered for a &quot;Home Cinema&quot; device can occupy several Gb and will be unreadable on a smartphone.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The same video rendered for a smartphone will require only one Mb, and can be played on a &quot;Home Cinema&quot; device but it will look ugly.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>That is why we recommend that you preserve your original project files as far as possible, and create from them videos suitable for the devices you wish to use for playback.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To simplify settings for the rendering operation, ffDiaporama contains predefined settings for the following devices:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;a href=&quot;0032.html&quot;&gt;Smartphones&lt;/a&gt; and &lt;a href=&quot;0033.html&quot;&gt;portable devices&lt;/a&gt;, and small video-playing devices in general</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;a href=&quot;0034.html&quot;&gt;Multimedia systems&lt;/a&gt; for domestic use (&quot;Home Cinema&quot; devices)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;a href=&quot;0035.html&quot;&gt;Display on the Web&lt;/a&gt; (blogs, picture-sharing and social networking sites)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>It is also possible for you to define all parameters directly by using the advanced option.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0032</name>
    <message>
        <source>Smartphones and tablets</source>
        <translation type="unfinished">Смартфоны и планшеты</translation>
    </message>
    <message>
        <source>Generally speaking the main differences between these devices are:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Screen size and maximal resolution</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Operating system on the device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Consult the documentation for your equipment or search the Internet to determine these two characteristics.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Device type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Operating system</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Available resolution</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remarks</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Smartphone</source>
        <translation type="unfinished">Смартфон</translation>
    </message>
    <message>
        <source>Nokia Symbian System S60</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>QVGA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>VGA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Generally, Nokia phones working with the Symbian operating system and equipped with a camera, are capable of playing videos. Very often, those provided with a keyboard offer a 320x240 resolution (QVGA), while those with only a touch-sensitive screen offer a 640x360 resolution (VGA).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Movie format is: Container: MP4 - Video Codec: MPEG4 - Audio Codec: MP3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Note: Video files should be stored in the &quot;My videos&quot; folder after connecting the telephone to the PC in &quot;mass storage&quot; mode. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Windows Mobile 6.0/6.1/6.5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>HVGA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>WVGA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Numerous manufacturers made phones using the Microsoft Windows Mobile 6 operating system (HP, HTC, etc.) between 2006 and 2010.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Movie format is: Container: 3GP - Video Codec: MPEG4 - Audio Codec: AMR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Note: Store video files in the &quot;My videos&quot; folder having connected the telephone to the PC in &quot;mass storage&quot; mode. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Windows Phone 7</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Microsoft Windows Phone 7 is the evolution of Windows Mobil 6</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Movie format is: Container: MP4 - Video Codec: h264 - Audio Codec: AAC-LC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Android</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1.5/1.6</source>
        <translation type="unfinished">1.5/1.6</translation>
    </message>
    <message>
        <source>2.1/2.2/2.3</source>
        <translation type="unfinished">2.1/2.2/2.3</translation>
    </message>
    <message>
        <source>Many manufacturers made or still make phones using the Google Android operating system.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Apple iOS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>iPhone 3G/3GS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>iPhone 4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>HP - WebOs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>WebOs arises from the repurchase of Palm by HP.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;To be completed&lt;/B&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Blackberry OS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>RIM 240</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Movie format is: Container: AVI - Video Codec: MPEG4 - Audio Codec: MP3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Note: store video files in the \Blackberry\Video folder after connecting the telephone to the PC &quot;mass storage&quot; mode.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Samsung BADA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Tablet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Android 2.x</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Android 3.x</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Linux</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>800x480</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>854x480</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Tablet examples: Archos 4/5/70/Arnova 10, Dell Streak, Creative ZiiO 7</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1024x600</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>XGA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Tablet examples: Archos 101, Samsung Galaxy Tab, HTC Flyer, Hannspree HANNSpad SN10T, Toshiba Folio 100</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1024x768</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Tablet examples: HP TouchPad</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1280x800</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>720p</source>
        <translation type="unfinished">720p</translation>
    </message>
    <message>
        <source>Tablet examples: ASUS Eee Pad, Motorola Xoom, Packard Bell Liberty Tab, LG Optimus Pad, Archos G9</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>iPad</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>RIM BlackBerry</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Playbook</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Netbook/NetPC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Windows/XP</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Require xVid or DivX codec to be installed on the computer.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Windows/Vista</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Windows/7</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Android/Linux</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>As far as the resolution is concerned, note that most devices can play videos designed for lower resolution devices without difficulty. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The device models available in ffDiaporama are as follows: </source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0033</name>
    <message>
        <source>Portable devices</source>
        <translation type="unfinished">Портативные устройства</translation>
    </message>
    <message>
        <source>These portable devices are usually very specific. We will not discuss therefore either operating system or resolution.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Device type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Manufacturer/Model</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remark</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Portable player</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>DVD/DivX portable player (car player or travel player)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>All these devices use the old DVD/DIVX format with anamorphic resolution 720x576 (PAL) or 720x480 ( NTSC).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Movie format is: Container: AVI - Video Codec: MPEG4 - Audio Codec: MP3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If the device does not have a USB port, burn a DVD from your video files. Otherwise simply copy your files to a USB pendrive.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>iPod Classic (G5/G6)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>iPod Nano (G3/G4/G5)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>These two portable player were especially designed for music. Therefore, their resolution is only QVGA.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Movie format is: Container: MP4 - Video Codec: h264 - Audio Codec: AAC-LC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Apple iPod Touch</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This portable player has a video resolution comparable to the DVD (without anamorphism).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Portable LED/LCD TV</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>These devices, when they support the HD TV, have a real resolution of 1024x576.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Handheld game console</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Sony PSP</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Resolution 320x480 / NTSC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>-s 320x480 -b 512000 -ar 24000 -ab 64000 -f psp -r 29.97</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>-ac 2 -ar 48000 -acodec libfAAC-LC -ab 128k -r FRAME_RATE -s WIDTHxHEIGHT -vcodec libh264 -vpre slower -vpre main -level 21 -refs 2 -b BIT_RATE -bt BIT_RATE -aspect WIDTH:HEIGHT -f psp</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0034</name>
    <message>
        <source>Multimedia system (Home cinema)</source>
        <translation type="unfinished">Мультимедиа система (домашний кинотеатр)</translation>
    </message>
    <message>
        <source>Device type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Manufacturer/Model</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remark</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>TV box of ADSL box</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>France - Livebox (Orange)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>All these devices support the old DVD/DIVX format with anamorphic resolution 720x576 (PAL) for the DVD resolution</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Movie format is: Container: AVI - Video Codec: MPEG4 - Audio Codec: MP3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>All these devices support the MP4 format up to the FullHD 1080p resolution</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Movie format is: Container: MP4 - Video Codec: h264 - Audio Codec: AAC-LC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Copy your video files to a USB pendrive or an external hard disk and use the multimedia player integrated into the menu of the decoder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>France - Freebox</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>France - Neufbox (SFR)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>France - Bbox (Bouygues Telecom)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Multimedia hard drive and gateway</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>All manufacturers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Equipments supporting HD (to verify in the technical specifications of the device) support the MP4 format until the FullHD 1080p resolution</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Player</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>DVD/DivX Player</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>These devices only support the old DVD/DIVX format with anamorphic resolution 720x576 (PAL) for the DVD resolution</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If the device does not have an USB connector: burn a DVD with your video files. Otherwise simply copy your videos files to a USB pendrive.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>BlueRay player</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;To be completed&lt;/B&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Games console</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>PS3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>XBox360</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> These &quot;Home Cinema&quot; devices are connected to a television set. The format choice will thus be made according to the requirements of the set used. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Warning: The old SD format is only suitable for old television sets using cathode ray tube technology. With these tubes, points were not circular but oval: we call this anamorphism. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Playing a video in SD format on a modern LCD, LED or plasma television set gives openly disastrous results: The image is particularly ugly! Thus, if your television is new and if your device allows it, always use a HD or Full-HD format in preference to a SD format. </source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0035</name>
    <message>
        <source>For the WEB</source>
        <translation type="unfinished">Интернет</translation>
    </message>
    <message>
        <source>Device type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Manufacturer/Model</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remark</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Flash player (SWF)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>All SWF players</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To be used with the HTML tag object class=&quot;player&quot; type=&quot;application/x-shockwave-flash&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>- For 4/3 : 320x240, 640x480, 960x720, 1440x1080</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>- For 16/9 : 320x180, 640x360, 1280x720, 1920x1080</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>- For 40/17 : 320x136, 640x272, 1280x544, 1920x816</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Movie format is: Container: SWF(2008) - Video Codec: h264 - Audio Codec: AAC-LC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Sharing and social networking</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Youtube</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>720p - Movie format is: Container: MP4 - Video Codec: xVid - Audio Codec: MP3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Daily Motion</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>720p - Movie format is: Container: MP4 - Video Codec: h264 - Audio Codec: AAC-LC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Facebook</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Publish the video on YouTube or Daily Motion and integrate a link towards this video into Facebook</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>HTML 5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>webm</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To be added to allow the new video TAG in HTML5 browsers. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>WebM is an open source format usable with Firefox, Google Chrome and Opera </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>H264 is a format usable in Safari, Internet Explorer and Google Chrome </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To maintain backward compatibility with older browsers, it is advisable to offer a flash version within the tag. See: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>developer.mozilla.org</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>mp4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>ogv</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0036</name>
    <message>
        <source>Video codec</source>
        <translation>Видео кодек</translation>
    </message>
    <message>
        <source>The following table lists the video codecs which ffDiaporama can use to generate videos:</source>
        <translation>В следующей таблице перечислены видео кодеки, которые ffDiaporama может использовать при создании видео:</translation>
    </message>
    <message>
        <source>Codec</source>
        <translation>Кодек</translation>
    </message>
    <message>
        <source>Name</source>
        <translation>Название</translation>
    </message>
    <message>
        <source>WIKIPEDIA/Remark</source>
        <translation>Википедия / Примечание</translation>
    </message>
    <message>
        <source>MPJEP</source>
        <translation>MPJEP</translation>
    </message>
    <message>
        <source>Motion JPEG</source>
        <translation>Motion JPEG</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Motion_JPEG</source>
        <translation>http://ru.wikipedia.org/wiki/MJPEG</translation>
    </message>
    <message>
        <source>MPEG2</source>
        <translation>MPEG2</translation>
    </message>
    <message>
        <source>MPEG-2 video</source>
        <translation>MPEG-2 video</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/MPEG-2</source>
        <translation>http://ru.wikipedia.org/wiki/MPEG-2</translation>
    </message>
    <message>
        <source>MPEG4</source>
        <translation>MPEG4</translation>
    </message>
    <message>
        <source>DivX/XVid/MPEG-4</source>
        <translation>DivX/XVid/MPEG-4</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Xvid</source>
        <translation>http://ru.wikipedia.org/wiki/Xvid</translation>
    </message>
    <message>
        <source>H264HQ</source>
        <translation>H264HQ</translation>
    </message>
    <message>
        <source>High Quality H.264 AVC/MPEG-4 AVC</source>
        <translation>High Quality H.264 AVC/MPEG-4 AVC</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/H.264/MPEG-4_AVC</source>
        <translation>http://ru.wikipedia.org/wiki/H.264</translation>
    </message>
    <message>
        <source>H264PQ</source>
        <translation>H264PQ</translation>
    </message>
    <message>
        <source>Phone Quality H.264 AVC/MPEG-4 AVC</source>
        <translation>Phone Quality H.264 AVC/MPEG-4 AVC</translation>
    </message>
    <message>
        <source>X264LL</source>
        <translation>X264LL</translation>
    </message>
    <message>
        <source>x264 Lossless</source>
        <translation>x264 Lossless</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/X264</source>
        <translation>http://ru.wikipedia.org/wiki/X264</translation>
    </message>
    <message>
        <source>VP8</source>
        <translation>VP8</translation>
    </message>
    <message>
        <source>WebP (WebM-VP8)</source>
        <translation>WebP (WebM-VP8)</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/WebM</source>
        <translation>http://ru.wikipedia.org/wiki/WebM</translation>
    </message>
    <message>
        <source>H263</source>
        <translation>H263</translation>
    </message>
    <message>
        <source>Sorenson H.263</source>
        <translation>Sorenson H.263</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/H.263</source>
        <translation>http://ru.wikipedia.org/wiki/H.263</translation>
    </message>
    <message>
        <source>THEORA</source>
        <translation>THEORA</translation>
    </message>
    <message>
        <source>Theora VP3</source>
        <translation>Theora VP3</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Theora</source>
        <translation>http://ru.wikipedia.org/wiki/Theora</translation>
    </message>
    <message>
        <source>ffDiaporama uses specific settings for encoding with x264. This settings are including: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>- &lt;B&gt;High Quality&lt;/B&gt; for &quot;Home Cinema&quot; devices based on x264 &lt;U&gt;main&lt;/U&gt; profile, with 3 refs and 3 bframes. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>- &lt;B&gt;Phone Quality&lt;/B&gt; for mobile devices based on x264 &lt;U&gt;baseline&lt;/U&gt; profile, with 3 refs and 0 bframes. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Both settings are using &lt;U&gt;veryfast&lt;/U&gt; preset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>ffDiaporama uses the standard x264 &lt;U&gt;fast&lt;/U&gt; preset with 3 refs and qp set to 0.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0037</name>
    <message>
        <source>Audio codec</source>
        <translation>Аудио кодек</translation>
    </message>
    <message>
        <source>The following table lists the audio codecs which ffDiaporama can use to generate videos:</source>
        <translation>В следующей таблице перечислены аудио кодеки, которые ffDiaporama может использовать при создании видео:</translation>
    </message>
    <message>
        <source>Codec</source>
        <translation>Кодек</translation>
    </message>
    <message>
        <source>Name</source>
        <translation>Название</translation>
    </message>
    <message>
        <source>WIKIPEDIA</source>
        <translation>Википедия</translation>
    </message>
    <message>
        <source>PCM</source>
        <translation>PCM</translation>
    </message>
    <message>
        <source>WAV / PCM signed 16-bit little-endian</source>
        <translation>WAV / PCM signed 16-bit little-endian</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Pulse-code_modulation</source>
        <translation>http://ru.wikipedia.org/wiki/%D0%98%D0%BC%D0%BF%D1%83%D0%BB%D1%8C%D1%81%D0%BD%D0%BE-%D0%BA%D0%BE%D0%B4%D0%BE%D0%B2%D0%B0%D1%8F_%D0%BC%D0%BE%D0%B4%D1%83%D0%BB%D1%8F%D1%86%D0%B8%D1%8F</translation>
    </message>
    <message>
        <source>MP3</source>
        <translation>MP3</translation>
    </message>
    <message>
        <source>MPEG-1/2 Audio Layer III</source>
        <translation>MPEG-1/2 Audio Layer III</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/MP3</source>
        <translation>http://ru.wikipedia.org/wiki/MP3</translation>
    </message>
    <message>
        <source>AAC-LC</source>
        <translation>AAC-LC</translation>
    </message>
    <message>
        <source>Advanced Audio Codec</source>
        <translation>Advanced Audio Codec</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Advanced_Audio_Coding</source>
        <translation>http://ru.wikipedia.org/wiki/Advanced_Audio_Coding</translation>
    </message>
    <message>
        <source>AC3</source>
        <translation>AC3</translation>
    </message>
    <message>
        <source>Dolby Digital</source>
        <translation>Dolby Digital</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Dolby_Digital</source>
        <translation>http://ru.wikipedia.org/wiki/Dolby_Digital</translation>
    </message>
    <message>
        <source>VORBIS</source>
        <translation>VORBIS</translation>
    </message>
    <message>
        <source>Vorbis</source>
        <translation>Vorbis</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Vorbis</source>
        <translation>http://ru.wikipedia.org/wiki/Vorbis</translation>
    </message>
    <message>
        <source>MP2</source>
        <translation>MP2</translation>
    </message>
    <message>
        <source>MPEG-1 Layer II</source>
        <translation>MPEG-1 Layer II</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/MPEG-2</source>
        <translation>http://ru.wikipedia.org/wiki/MPEG-2</translation>
    </message>
    <message>
        <source>AMR</source>
        <translation>AMR</translation>
    </message>
    <message>
        <source>Adaptive Multi-Rate</source>
        <translation>Adaptive Multi-Rate</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Adaptive_Multi-Rate_audio_codec</source>
        <translation>http://ru.wikipedia.org/wiki/AMR_%28%D1%81%D0%B6%D0%B0%D1%82%D0%B8%D0%B5_%D0%B7%D0%B2%D1%83%D0%BA%D0%B0%29</translation>
    </message>
    <message>
        <source>FLAC</source>
        <translation>FLAC</translation>
    </message>
    <message>
        <source>Free Lossless Audio Codec</source>
        <translation>Free Lossless Audio Codec</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Flac</source>
        <translation>http://ru.wikipedia.org/wiki/FLAC</translation>
    </message>
</context>
<context>
    <name>0038</name>
    <message>
        <source>Container</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The following table lists the containers which ffDiaporama can use to generate videos, as well as the available audio and video codecs for every container:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Extension</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Name</source>
        <translation type="unfinished">Название</translation>
    </message>
    <message>
        <source>Video Codecs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Audio Codecs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>WIKIPEDIA</source>
        <translation type="unfinished">Википедия</translation>
    </message>
    <message>
        <source>3GP</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>MPEG4, H264HQ/H264PQ</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>AMR</source>
        <translation type="unfinished">AMR</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/3GP_and_3G2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>AVI</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Audio Video Interleave</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>MJPEG, MPEG2, MPEG4, H264HQ/H264PQ</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>PCM, MP2, MP3, AAC, AC3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Audio_Video_Interleave</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>MKV</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Matroska file format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>MPEG4, H264HQ/H264PQ, THEORA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>PCM, MP3, AAC, AC3, VORBIS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Matroska</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>MJPEG</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Motion JPEG</source>
        <translation type="unfinished">Motion JPEG</translation>
    </message>
    <message>
        <source>PCM</source>
        <translation type="unfinished">PCM</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Motion_JPEG</source>
        <translation type="unfinished">http://ru.wikipedia.org/wiki/MJPEG</translation>
    </message>
    <message>
        <source>MP4</source>
        <translation type="unfinished">MP4</translation>
    </message>
    <message>
        <source>MPEG-4 Part 14</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>MP3, AAC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/MPEG-4_Part_14</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>MPEG</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>MPEG program stream</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>MPEG2</source>
        <translation type="unfinished">MPEG2</translation>
    </message>
    <message>
        <source>MP2</source>
        <translation type="unfinished">MP2</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/MPEG_program_stream</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>WEBM</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>WebM</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>VP8</source>
        <translation type="unfinished">VP8</translation>
    </message>
    <message>
        <source>VORBIS</source>
        <translation type="unfinished">VORBIS</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/WebM</source>
        <translation type="unfinished">http://ru.wikipedia.org/wiki/WebM</translation>
    </message>
    <message>
        <source>FLV</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Flash file format 2005</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>H263</source>
        <translation type="unfinished">H263</translation>
    </message>
    <message>
        <source>MP3</source>
        <translation type="unfinished">MP3</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Adobe_Flash</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Flash file format 2008</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>H264HQ/H264PQ</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>AAC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>OGV</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Ogg</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>THEORA</source>
        <translation type="unfinished">THEORA</translation>
    </message>
    <message>
        <source>http://en.wikipedia.org/wiki/Ogg</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0039</name>
    <message>
        <source>Video standards and resolutions</source>
        <translation type="unfinished">Видео стандарты и разрешения</translation>
    </message>
    <message>
        <source> Standards and resolution are both fundamental properties which define a video. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Two associated properties are the geometry and the FPS number. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Standard</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Standards are a historic feature which continues today in spite of technological progress. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A rich literature on this subject can be found on the internet. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To simplify things we shall summarize as follows: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>PAL is effective in all the countries where the electricity network is 50 hz (which is the case, for example, for Western European countries)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>NTSC is effective in all the countries where the electricity network is 60 hz (which is the case, for example, for the United States of America)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Geometry</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> The geometry is the ratio of Width to Height. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>4:3 corresponds to screens that are almost square</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>16:9 corresponds to screens that are rectangular</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>40:17 corresponds to cinema screens in overall size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Resolution</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Unlike with photos, video resolution is not expressed in megapixels but in number of points (pixels) shown by the screen multiplied by the number of lines. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>So, 320x240 corresponds to 320 points of width and 240 lines in height </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>FPS (Frames per second)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Corresponds to the number of images shown every second. You should know that in cinemas, 24 images are shown per second and this figure of 24 images is recognized as corresponding to the maximum number of images which the eye is capable of differentiating: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>More images per second and the eye does not notice it</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Fewer images per second and the film appears jerky</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Matrix of combinations managed by ffDiaporama</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> The following table indicates all the combinations of geometry, standards and resolutions managed by ffDiaporama. For every combination, it indicates the number of associated FPS: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Name</source>
        <translation type="unfinished">Название</translation>
    </message>
    <message>
        <source>PAL</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>NTSC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remark</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>FPS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>RIM 240</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>4:3</source>
        <translation>4:3</translation>
    </message>
    <message>
        <source>240x180</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>24 FPS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Special format used on Blackberry&apos;s very small screen</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>6:9</source>
        <translation type="unfinished">6:9</translation>
    </message>
    <message>
        <source>240x136</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>40:17</source>
        <translation type="unfinished">40:17</translation>
    </message>
    <message>
        <source>QVGA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>320x240</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>25 FPS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>29,97 FPS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Used mainly on the Internet and by first generation smartphones and mobile video players.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>16:9</source>
        <translation type="unfinished">16:9</translation>
    </message>
    <message>
        <source>320x180</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>320x136</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>HVGA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>426x320</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Used mainly on the Internet and by second generation smartphones and mobile video players. &lt;B&gt;Because this format does not respect the traditional video geometry&lt;/B&gt;, &lt;B&gt;black bands appear to fill the screen to 480x320.&lt;/B&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>480x270</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>480x204</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>VGA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>640x480</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Used mainly on the Internet and by some smartphones.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>640x360</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>640x272</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>SD-DVD</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>720x576</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>720x480</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The old television format for cathode ray tubes. Note that this format was specifically created for equipment with oval points: It is the anamorphism of the points that determines the geometry. That is why &lt;B&gt;it is absolutely necessary to avoid this format for anything other than television sets with cathode ray tubes.&lt;/B&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>WVGA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Used mainly on the Internet and by new generation smartphones and mobile video players. This format has become widespread and has replaced the old  QVGA, HVGA and VGA format used in the years 2000-2010. &lt;B&gt;Because this format does not respect the traditional video geometries, black bands appear to fill the screen to 800x480.&lt;/B&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>800x450</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>800x340</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>XGA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1024x768</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Historically this format was used only by computers (fixed or portable). Now we meet it more and more on tablet computers like the iPad.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1024x576</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1024x436</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>720p</source>
        <translation type="unfinished">720p</translation>
    </message>
    <message>
        <source>960x720</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>23,976 FPS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Today, this format is widely used on the Internet, in particular by social networking and video-sharing sites. We also find it on certain &quot;home cinema&quot; devices.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1280x720</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1280x544</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1080p</source>
        <translation type="unfinished">1080p</translation>
    </message>
    <message>
        <source>1440x1080</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This is the real &quot;home cinema&quot; format. It is used by the Blue Ray players and the &quot;home cinema&quot; devices.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1920x1080</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1920x816</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Note that many other resolutions exists but which are not used by ffDiaporama.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>003A</name>
    <message>
        <source>File attachments (Thumbnails, nfo, etc...)</source>
        <translation>Присоединяемые файлы (обложка, nfo, и др.)</translation>
    </message>
    <message>
        <source>ffDiaporama allows you to produce joint files during the rendering process.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The following files are availables:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Thumbnails</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A video or music file can be accompanied with an image file which must have the same name but with the .jpg extension (example: holidays.mkv and holidays.jpg in the same folder). </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This image is called a thumbnail and is used as icon for this file by ffDiaporama and many </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Media Center Systems</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>XBMC .nfo files</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The .nfo file for XBMC allows XBMC to recognize the rendered videos as movies. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>For more information see: &lt;a href=&quot;0044.html&quot;&gt;XBMC nfo files&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0040</name>
    <message>
        <source>Technical information on ffDiaporama</source>
        <translation>Техническая информация о ffDiaporama</translation>
    </message>
    <message>
        <source>ffDiaporama command line arguments</source>
        <translation>Параметры командной строки ffDiaporama</translation>
    </message>
    <message>
        <source>ffDiaporama project files</source>
        <translation>Файлы проектов ffDiaporama</translation>
    </message>
    <message>
        <source>The configuration files for ffDiaporama</source>
        <translation>Файлы конфигурации ffDiaporama</translation>
    </message>
    <message>
        <source>XBMC nfo files</source>
        <translation>XBMC nfo файлы</translation>
    </message>
    <message>
        <source>ffDiaporama WIKI - Technical information on ffDiaporama</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This section contains technical information about ffDiaporama. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Index</source>
        <translation type="unfinished">Содержание</translation>
    </message>
</context>
<context>
    <name>0041</name>
    <message>
        <source>ffDiaporama command line arguments</source>
        <translation type="unfinished">Параметры командной строки ffDiaporama</translation>
    </message>
    <message>
        <source>Syntax : ffDiaporama [-lang=] [-loglevel=] [Filename]</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Setting</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Explanation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Filename</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Filename of the project file to load at application startup</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>-lang=</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The language to be used. can take the following values: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;cz&lt;/B&gt; to use Czech</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;de&lt;/B&gt; to use German</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;en&lt;/B&gt; to use English</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;es&lt;/B&gt; to use Spanish</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;el&lt;/B&gt; to use Greek</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;fr&lt;/B&gt; to use French</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;it&lt;/B&gt; to use Italian</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;nl&lt;/B&gt; to use Dutch</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;pt&lt;/B&gt; to use Portuguese</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;ru&lt;/B&gt; to use Russian</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;zh-tw&lt;/B&gt; to use Taiwanese</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>-loglevel=</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Adjust the level of messages shown in the console: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>1 to show all the messages of Debug, Information, Warning and Error levels</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>2 to show the messages of Information, Warning and Error levels</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>3 to show the messages of Warning and Error levels</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>4 to show only the messages of Error levels</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Note: messages appear only if you launched the program from a console (CMD under Windows or Terminal under Linux). It is possible to redirect these messages into a log file by using &gt;. </source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0042</name>
    <message>
        <source>ffDiaporama project files</source>
        <translation>Файлы проектов ffDiaporama</translation>
    </message>
    <message>
        <source>Project files for ffDiaporama are XML files with .ffd extension.</source>
        <translation>Файлы проектов ffDiaporama - это XML файлы с расширением .ffd.</translation>
    </message>
    <message>
        <source>This XML file does not contain copies of the multimedia elements (image, video, music) which are included in the project but only links to these files.</source>
        <translation>Этот XML-файл не содержит копии мультимедийных элементов (изображения, видео, музыка), которые включены в проект, там только ссылки на эти файлы.</translation>
    </message>
    <message>
        <source>The links are relative to the location of the .ffd file. This ensures that if the directory containing the project file (.ffd ) also contains the multimedia elements (directly or in subdirectories), it is possible to move and/or to rename the directory.</source>
        <translation>Ссылки формируются относительно расположения файла .ffd. Это гарантирует, что если каталог, содержащий файл проекта (.ffd), также содержит мультимедийные элементы (непосредственно или в подкаталогах), то его можно переместить или переименовать.</translation>
    </message>
    <message>
        <source>Another advantage is that a project created under Linux can be opened under Windows and conversely.</source>
        <translation>Еще одним преимуществом является то, что проект, созданный в Linux, может быть открыт в Windows и наоборот.</translation>
    </message>
</context>
<context>
    <name>0043</name>
    <message>
        <source>The configuration files for ffDiaporama</source>
        <translation>Файлы конфигурации ffDiaporama</translation>
    </message>
    <message>
        <source>The primary ffDiaporama.xml file is stored in the ffDiaporama directory. This file is global for all users: it contains the default ffDiaporama application settings. </source>
        <translation>Основная файл ffDiaporama.xml хранится в папке ffDiaporama. Этот файл является общим для для всех пользователей: он содержит параметры ffDiaporama по умолчанию.</translation>
    </message>
    <message>
        <source>For Windows, this is C:\Program Files\ffDiaporama</source>
        <translation>Для Windows это C:\Program Files\ffDiaporama</translation>
    </message>
    <message>
        <source>For Linux, it is /usr/share/ffDiaporama</source>
        <translation>Для Linux это /usr/share/ffDiaporama</translation>
    </message>
    <message>
        <source>A secondary user-specific ffDiaporama.xml file is stored in the user&apos;s profile folder. </source>
        <translation>Вторичный (для каждого конкретного пользователя) файл ffDiaporama.xml хранится в папке профиля пользователя.</translation>
    </message>
    <message>
        <source>For Windows, this is C:\Document and settings\[user]</source>
        <translation>Для Windows это C:\Document and settings\[user]</translation>
    </message>
    <message>
        <source>For Linux, it is ~/.ffDiaporama</source>
        <translation>Для Linux это ~/.ffDiaporama</translation>
    </message>
    <message>
        <source> When the application is launched for the first time, the user configuration file is created if it does not already exist. </source>
        <translation>При первом запуске приложения создается пользовательский файл конфигурации, если он еще не существует.</translation>
    </message>
    <message>
        <source>Remark: </source>
        <translation>Примечание: </translation>
    </message>
    <message>
        <source>If ffDiaporama is used by several users on the same PC, there will be a separate configuration file for each user.</source>
        <translation>Если ffDiaporama используется несколькими пользователями на одном компьютере , то для каждого пользователя будет создан отдельный файл конфигурации.</translation>
    </message>
    <message>
        <source> The same principle is used for 2 files: </source>
        <translation>Тот же принцип используется для 2 файлов:</translation>
    </message>
    <message>
        <source>File</source>
        <translation>Файл</translation>
    </message>
    <message>
        <source>Use</source>
        <translation>Назначение</translation>
    </message>
    <message>
        <source>ffDiaporama.xml</source>
        <translation>ffDiaporama.xml</translation>
    </message>
    <message>
        <source>Contains the application options for ffDiaporama</source>
        <translation>Содержит настройки ffDiaporama</translation>
    </message>
    <message>
        <source>Devices.xml</source>
        <translation>Devices.xml</translation>
    </message>
    <message>
        <source>Contains the base of profiles for video generation.</source>
        <translation>Содержит базу профилей устройств для создания видео.</translation>
    </message>
    <message>
        <source>In order to work, the application needs two configuration files. Each of them consists of a pair of files.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>For example:</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0044</name>
    <message>
        <source>XBMC nfo files</source>
        <translation>XBMC nfo файлы</translation>
    </message>
    <message>
        <source>The .nfo file for XBMC allows XBMC to recognize the rendered videos as movies.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>When doing a scan or an automatic new file discovery, XBMC uses the .nfo files.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>During a manual refresh, XBMC offers you to find the film through one of its scrapers. Refuse its proposal to use the information contained in the .nfo file.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Information included in the .nfo file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot; standalone=&quot;yes&quot; ?&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;movie&gt; </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;title&gt;&lt;U&gt;&lt;B&gt;Project Title&lt;/B&gt;&lt;/U&gt;&lt;/title&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;sorttitle&gt;&lt;U&gt;&lt;B&gt;Rendered filename&lt;/B&gt;&lt;/U&gt;&lt;/sorttitle&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;set&gt;&lt;B&gt;&lt;U&gt;Project Album&lt;/U&gt;&lt;/B&gt;&lt;/set&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;genre&gt;&lt;B&gt;&lt;U&gt;Project Album&lt;/U&gt;&lt;/B&gt;&lt;/genre&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;year&gt;&lt;U&gt;&lt;B&gt;Year of project date&lt;/B&gt;&lt;/U&gt;&lt;/year&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;outline&gt;&lt;U&gt;&lt;B&gt;Project Title&lt;/B&gt;&lt;/U&gt;&lt;/outline&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;plot&gt;&lt;U&gt;&lt;B&gt;Project Comment&lt;/B&gt;&lt;/U&gt;&lt;/plot&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;director&gt;&lt;U&gt;&lt;B&gt;Project Author&lt;/B&gt;&lt;/U&gt;&lt;/director&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;credits&gt;&lt;U&gt;&lt;B&gt;Project Author&lt;/B&gt;&lt;/U&gt;&lt;/credits&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;runtime&gt;&lt;U&gt;&lt;B&gt;Project duration in minutes&lt;/B&gt;&lt;/U&gt;&lt;/runtime&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;thumb&gt;&lt;U&gt;&lt;B&gt;Project thumbnail&lt;/B&gt;&lt;/U&gt;&lt;/thumb&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> &lt;/movie&gt; </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>See also</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Information on XBMC movies .nfo file</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0101</name>
    <message>
        <source>Application options dialog</source>
        <translation>Диалоговое окно Настройки программы</translation>
    </message>
    <message>
        <source>The Application options tab allows to define the general behavior of ffDiaporama.</source>
        <translation>Вкладка Параметры программы позволяет определить общее поведение ffDiaporama.</translation>
    </message>
    <message>
        <source>The Project options tab allows to define the options by default of the current project and for the new projects.</source>
        <translation>Вкладка Параметры проекта позволяет определить настройки по умолчаню для текущего проекта и новых проектов.</translation>
    </message>
    <message>
        <source>The Rendering options Tab allows to define the options used by default during the generation of videos.</source>
        <translation>Вкладка Параметры создания файла позволяет определить настройки, используемые по умолчанию, при создании видео.</translation>
    </message>
    <message>
        <source>Application options</source>
        <translation>Параметры программы</translation>
    </message>
    <message>
        <source>Remember Windows size and positions</source>
        <translation>Запоминать размер и позицию окон</translation>
    </message>
    <message>
        <source>If this box is checked, the sizes and the positions of all windows and dialogs will be saved and reused the next time you use ffDiaporama.</source>
        <translation>Если этот флажок установлен, размеры и положение всех окон и диалогов будут запомнены и повторно использованы при следующем запуске ffDiaporama.</translation>
    </message>
    <message>
        <source>Remember last used folders</source>
        <translation>Запоминать последнюю используемую папку</translation>
    </message>
    <message>
        <source>If this box is checked, the directories for images, videos, projects, music and the generation of your movie will be saved and reused the next time you use ffDiaporama.</source>
        <translation>Если этот флажок установлен, папки с изображениями, видео, проектами, музыкой и созданным фильмом будут запомнены и повторно использованы при следующем запуске ffDiaporama.</translation>
    </message>
    <message>
        <source>Enable &quot;raster&quot; Graphics system</source>
        <translation>Включить &quot;растровую&quot; Графическую Систему</translation>
    </message>
    <message>
        <source>&lt;B&gt;For Linux users only: &lt;/B&gt;If this box is checked, the graphic system &quot;raster&quot; is used instead of the default graphic system. This option is used only at application start up. </source>
        <translation>&lt;B&gt;Только для пользователей Linux: &lt;/B&gt;если этот флажок установлен, будет использоваться растровая  графическая система вместо графической системы по умолчанию. Этот параметр вступит в силу только после перезапуска программы.</translation>
    </message>
    <message>
        <source>The &quot;raster&quot; Graphics accelerates the display but can raise problems on some computer in particular with KDE.</source>
        <translation>Растровая графика ускоряет отображение, но может вызвать проблемы на некоторых компьютерах, в частности с KDE.</translation>
    </message>
    <message>
        <source>SDL Audio use old mode</source>
        <translation>Использовать старый аудио режим SDL</translation>
    </message>
    <message>
        <source>&lt;B&gt;For the users Linux only: &lt;/B&gt;if this box is checked, old setting mode is use for SDL Audio. This option is useful on some Linux distribution which still use the old SDL packages (For example: Ubuntu 10.10 and 11.04, Fedora 14 and 15, etc.).</source>
        <translation>&lt;B&gt;Только для пользователей Linux: &lt;/B&gt;если этот флажок установлен, используется старый режим SDL аудио. Этот параметр полезен на некоторых дистрибутивах Linux, которые все еще используют старые пакеты SDL (например: Ubuntu 10.10 и 11.04, Fedora 14 и 15, и т.д.).</translation>
    </message>
    <message>
        <source>Memory profil</source>
        <translation>Профиль памяти</translation>
    </message>
    <message>
        <source>Preview options</source>
        <translation>Настройка предварительного просмотра</translation>
    </message>
    <message>
        <source>Frame rate</source>
        <translation>Частота кадров</translation>
    </message>
    <message>
        <source>Defines the number of images per second which will be generated to show the preview. </source>
        <translation>Определяет количество кадров в секунду, которое будет использоваться при предварительном просмотре.</translation>
    </message>
    <message>
        <source>Note: the higher this number, the more smooth the preview will be. On the other hand, more computer resources will be required. Reduce this number on slow computers or if the gauge for your multimedia reader is frequently in the red zone.</source>
        <translation>Примечание: чем выше это число, тем более гладким будет просмотр. С другой стороны потребуется больше ресурсов компьютера. Уменьшите это значение на медленных компьютерах или если датчик вашего мультимедиапроигрывателя часто находится в красной зоне.</translation>
    </message>
    <message>
        <source>Image smoothing (smooth images during preview)</source>
        <translation>Сглаживание изображений (при предварительном просмотре)</translation>
    </message>
    <message>
        <source>If this box is checked, the image smoothing effect will be applied to the images during the preview. </source>
        <translation>Если этот флажок установлен, то эффект сглаживания изображений будет применен во время предварительного просмотра.</translation>
    </message>
    <message>
        <source>Editor options</source>
        <translation>Параметры редактора</translation>
    </message>
    <message>
        <source>Coordinates unit</source>
        <translation>Единицы измерения</translation>
    </message>
    <message>
        <source>This option allows to select the unit for the coordinates which will be used in the properties dialog. You can choose: </source>
        <translation>Эта опция позволяет выбрать единицы измерения  координат, которые будут использоваться в диалоговом окне свойств. Вы можете выбрать: </translation>
    </message>
    <message>
        <source>Percentage: Coordinates (X, Y, Width and Height) are expressed in percentages</source>
        <translation>проценты: координаты (X, Y, ширина и высота) будут выражены в процентах</translation>
    </message>
    <message>
        <source>Pixel: Coordinates (X, Y, Width and Height) are expressed in pixels relative to a screen in 1080p mode</source>
        <translation>пикселы: координаты (X, Y, ширина и высота) будут выражены в пикселях относительно экрана в режиме 1080p</translation>
    </message>
    <message>
        <source>Object position</source>
        <translation>Положение объекта</translation>
    </message>
    <message>
        <source>This option allows you to adjust the positions occupied by new objects inserted into the timeline. </source>
        <translation>Этот параметр позволяет определить позицию, занимаемую новыми объектами, вставляемыми на линию времени. </translation>
    </message>
    <message>
        <source>If &lt;I&gt;&lt;B&gt;Insert after the current selected object&lt;/B&gt;&lt;/I&gt; is selected, then the new slides will be created after the current slide.</source>
        <translation>Если выбран параметр &lt;I&gt;&lt;B&gt;Вставить после текущего объекта&lt;/B&gt;&lt;/I&gt;, то новые слайды будут создаваться после текущего слайда.</translation>
    </message>
    <message>
        <source>If &lt;I&gt;&lt;B&gt;Append to the end of the project&lt;/B&gt;&lt;/I&gt; is selected, then the new slides will be created at the end of the project.</source>
        <translation>Если выбран параметр &lt;I&gt;&lt;B&gt;Добавить в конец проекта&lt;/B&gt;&lt;/I&gt;, то новые слайды будут создаваться в конце проекта.</translation>
    </message>
    <message>
        <source>Multiple file insertion</source>
        <translation>Добавление нескольких объектов</translation>
    </message>
    <message>
        <source>This option allows you to adjust the way files are sorted when you select several files in &lt;I&gt;&lt;B&gt;Add file&lt;/B&gt;&lt;/I&gt;. </source>
        <translation>Эта опция позволяет настроить способ сортировки при добавлении нескольких файлов с помощью кнопки &lt;I&gt;&lt;B&gt;Добавить файл&lt;/B&gt;&lt;/I&gt;. </translation>
    </message>
    <message>
        <source>If this box is checked, files will be added in order of their numerical suffix : Example: if you select IMG_0025, IMG_0032, MVI_0029 and MVI_0040, the files will be inserted as follows : IMG_0025, MVI_0029, IMG_0032 and MVI_0040</source>
        <translation>Если этот флажок установлен, то файлы будут добавлены в порядке их численных суффиксов: например, если вы выберете файлы IMG_0025, IMG_0032, MVI_0029 и MVI_0040, то они будут расположены в следующем порядке: IMG_0025, MVI_0029, IMG_0032 и MVI_0040</translation>
    </message>
    <message>
        <source>If this box is unchecked, files are added in alphabetical order.</source>
        <translation>Если этот флажок не установлен, то файлы будут добавлены в алфавитном порядке.</translation>
    </message>
    <message>
        <source>Object deletion</source>
        <translation>Удаление объекта</translation>
    </message>
    <message>
        <source>If this box is checked, a confirmation prompt will be displayed every time you remove an object: </source>
        <translation>Если этот флажок установлен, то запрос на подтверждение будет отображаться при каждом удалении объекта: </translation>
    </message>
    <message>
        <source>A slide from the main window</source>
        <translation>слайда из главного окна</translation>
    </message>
    <message>
        <source>A shot or bloc from the slide properties dialog box</source>
        <translation>кадра или блока из диалогового окна свойств слайда</translation>
    </message>
    <message>
        <source>If the &lt;B&gt;Activate deinterlace filter when add new MTS videos&lt;/B&gt; is checked, deinterlace filter will automatically be activated for all .mts videos added to the project. </source>
        <translation>Если установлен флажок &lt;B&gt;Устранение чересстрочности при добавлении нового MTS видео&lt;/B&gt;, то фильтр устранения чересстрочности будет автоматически активирован для всего .mts видео, добавленного в проект.</translation>
    </message>
    <message>
        <source>interlaced video</source>
        <translation>чересстрочность видео</translation>
    </message>
    <message>
        <source>Project options</source>
        <translation>Параметры проекта</translation>
    </message>
    <message>
        <source>Project properties</source>
        <translation>Свойства проекта</translation>
    </message>
    <message>
        <source>Title</source>
        <translation>Название</translation>
    </message>
    <message>
        <source>Defined the automatic filling method to use for the title&apos;s project properties: </source>
        <translation>Определяет метод автоматического заполнения названия проекта: </translation>
    </message>
    <message>
        <source>No automatic filling : You will have to define manually the title</source>
        <translation>Без автозаполнения: вы должны будете задать название вручную</translation>
    </message>
    <message>
        <source>Fill with project name when project save : The title will take the same name as the .ffd file</source>
        <translation>Использовать имя проекта при сохранении: название будет таким же, как имя ffd файла</translation>
    </message>
    <message>
        <source>Fill with project folder name when project save : The title will take the same name as the folder in witch is save the .ffd file</source>
        <translation>Использовать имя папки проекта при сохранении: название будет таким же, как имя папки, где находится ffd файл</translation>
    </message>
    <message>
        <source>Author</source>
        <translation>Автор</translation>
    </message>
    <message>
        <source>Default language</source>
        <translation>Язык по умолчанию</translation>
    </message>
    <message>
        <source>Defines the default value for the field Language</source>
        <translation>Определяет значение по умолчанию для поля Язык</translation>
    </message>
    <message>
        <source>New project geometry</source>
        <translation>Новая геометрия проекта</translation>
    </message>
    <message>
        <source>Selects the image geometry to be used for the creation of projects. Three geometries are available : </source>
        <translation>Выбор геометрии изображения, используемой при создании проектов. Доступно три геометрии:</translation>
    </message>
    <message>
        <source> Note: the geometry of a project is defined once and for all when it is created. It is not possible to modify it afterwards.</source>
        <translation>Примечание: геометрия проекта определяется при его создании один раз и навсегда. Ее невозможно изменить  впоследствии.</translation>
    </message>
    <message>
        <source>Default 1&lt;SUP&gt;st&lt;/SUP&gt; shot duration</source>
        <translation>Длительность 1&lt;SUP&gt;-го&lt;/SUP&gt; кадра по умолчанию</translation>
    </message>
    <message>
        <source>Defines the default duration of the first shot for slides during their creation.</source>
        <translation>Определяет длительность первого кадра  по умолчанию для слайдов во время их создания.</translation>
    </message>
    <message>
        <source>Next shot duration</source>
        <translation>Длительность следующего кадра</translation>
    </message>
    <message>
        <source>Defines the default duration of further shots.</source>
        <translation>Определяет длительность следующих кадров (по умолчанию).</translation>
    </message>
    <message>
        <source>Default transition</source>
        <translation>Переход по умолчанию</translation>
    </message>
    <message>
        <source>Defines the transition to be inserted between inserted slides : </source>
        <translation>Определяет основной тип перехода между слайдами: </translation>
    </message>
    <message>
        <source> A transition chosen randomly from the library</source>
        <translation>переход выбирается случайным образом</translation>
    </message>
    <message>
        <source> No transition</source>
        <translation>без перехода</translation>
    </message>
    <message>
        <source> Cross-fade transition</source>
        <translation>плавное затухание</translation>
    </message>
    <message>
        <source>Default options for speed waves</source>
        <translation>Параметры скорости по умолчанию</translation>
    </message>
    <message>
        <source>Transition</source>
        <translation>Переход</translation>
    </message>
    <message>
        <source>Sets the default speed wave for the transitions of new projects</source>
        <translation>Устанавливает скорость переходов новых проектов по умолчанию</translation>
    </message>
    <message>
        <source>Blocks animation</source>
        <translation>Анимация блока</translation>
    </message>
    <message>
        <source>Sets the default speed curve for the blocks animations of new projects</source>
        <translation>Устанавливает скорость анимации блоков новых проектов по умолчанию</translation>
    </message>
    <message>
        <source>Images animation</source>
        <translation>Анимация изображения</translation>
    </message>
    <message>
        <source>Sets the default speed for the image effects animations of new projects</source>
        <translation>Устанавливает скорость анимации изображений новых проектов по умолчанию</translation>
    </message>
    <message>
        <source>Default options for new text block</source>
        <translation>Параметры нового текстового блока по умолчанию</translation>
    </message>
    <message>
        <source>Text</source>
        <translation>Текст</translation>
    </message>
    <message>
        <source>Allows to select the characters style applied to the new text blocks. This style defines the font, its size, its color and the shadow effects applied to the text.</source>
        <translation>Позволяет выбрать стиль символов, применяемый к новым блокам текста. Этот стиль определяет шрифт, его размер, цвет и эффект тени, применяемый к тексту.</translation>
    </message>
    <message>
        <source>Background</source>
        <translation>Фон</translation>
    </message>
    <message>
        <source>Allows to select the style of background applied to the new text blocks. This style defines the transparency of the block and its filling.</source>
        <translation>Позволяет выбрать стиль фона, применяемый к новому текстовому блоку. Этот стиль определяет прозрачность блока и его заливку.</translation>
    </message>
    <message>
        <source>Shape</source>
        <translation>Форма</translation>
    </message>
    <message>
        <source>Coordinates</source>
        <translation>Координаты</translation>
    </message>
    <message>
        <source>Allows to select, for every project geometry, the style of the location applied to the new text blocks. This style defines the size, the coordinates, the rotations and the geometrical constraint.</source>
        <translation>Позволяет выбрать для каждой геометрии проекта стиль расположения, применяемый к новым текстовым блокам. Этот стиль определяет размер, координаты, вращение и ограничения геометрии.</translation>
    </message>
    <message>
        <source>Locking</source>
        <translation>Блокировка</translation>
    </message>
    <message>
        <source>Allows to Select the geometric constraint applied to the new text blocks. </source>
        <translation>Позволяет выбрать блокировку геометрии, применяемую к новым блокам текста. </translation>
    </message>
    <message>
        <source>Unlock: The shape of the block is free</source>
        <translation>Разблокировать: свободная форма блока</translation>
    </message>
    <message>
        <source>Lock to project geometry: The shape of the has the same shape as the image</source>
        <translation>Заблокировать геометрию проекта: форма блока имеет такую ​​же форму, что и изображение</translation>
    </message>
    <message>
        <source>Default options for new image or video block (when creating slide)</source>
        <translation>Параметры нового изображения или видео (по умолчанию - при создании слайда)</translation>
    </message>
    <message>
        <source>The following settings apply to all image formats:</source>
        <translation>Следующие настройки применяются ко всем форматам изображений:</translation>
    </message>
    <message>
        <source>Text style</source>
        <translation>Стиль текста</translation>
    </message>
    <message>
        <source>Allows to select the characters style applied to the text of the blocks. This style defines the font, its size, its color and the shadow effects applied to the text.</source>
        <translation>Позволяет выбрать стиль символов, применяемый к блокам текста. Этот стиль определяет шрифт, его размер, цвет и эффект тени, применяемый к тексту.</translation>
    </message>
    <message>
        <source>For every image format and every project geometry select the style of location to applied to the new blocks. This style concerns the size, the coordinates, the rotations and the geometrical constraint.</source>
        <translation>Для каждого формата изображения и каждой геометрии проекта выберите стиль расположения, применяемый к новым блокам. Этот стиль определяет размер, координаты, вращения и ограничения геометрии.</translation>
    </message>
    <message>
        <source>Framing styles define how to crop the image to make it compatible with the block in which is displayed.</source>
        <translation>Стиль кадрирования определяет  как обрезается изображение, для обеспечения его совместимости с блоком, в котором оно отображается.</translation>
    </message>
    <message>
        <source>The positioning styles define the the size, coordinates, rotations and geometric constraint to apply to blocks.</source>
        <translation>Стиль позиционирования определяет размер, координаты, вращение и ограничения геометрии, применяемые к блокам.</translation>
    </message>
    <message>
        <source>Default options for new image or video block (when adding block in slide dialog)</source>
        <translation>Параметры нового изображения или видео (по умолчанию - при добавлении блока)</translation>
    </message>
    <message>
        <source>The same selection that when creating slides apply.</source>
        <translation>Используются те же настройки, что и при создании слайдов.</translation>
    </message>
    <message>
        <source>Rendering options</source>
        <translation>Параметры создания файла</translation>
    </message>
    <message>
        <source>Default video name</source>
        <translation>Имя видео (по умолчанию)</translation>
    </message>
    <message>
        <source>This option allows to select the name which will be proposed by default during the rendering of videos. This option can be applied only if the project was saved at first.</source>
        <translation>Эта опция позволяет выбрать название, которое будет предложено по умолчанию во время создания видео. Она может быть применена только если проект был сохранен впервые.</translation>
    </message>
    <message>
        <source>Use project name as video name</source>
        <translation>Использовать имя файла проекта как название видео</translation>
    </message>
    <message>
        <source>If the project name is &quot;assembly.ffd&quot;, then &quot;assembly&quot; will be the proposed name</source>
        <translation>если имя файла проекта &quot;assembly.ffd&quot;, то название видео будет &quot;assembly&quot;</translation>
    </message>
    <message>
        <source>Use project folder&apos;s name as video name</source>
        <translation>Использовать имя папки проекта как название видео</translation>
    </message>
    <message>
        <source>If the project name is &quot;assembly.ffd&quot; and is saved in the &quot;My holidays 2011&quot; directory, then &quot;My holidays 2011&quot; will be the proposed name</source>
        <translation>если имя проекта &quot;assembly.ffd&quot; и он сохранен в папке &quot;Мои праздники 2011&quot;, то название видео будет &quot; Мои праздники 2011&quot;</translation>
    </message>
    <message>
        <source>Use title project as video name</source>
        <translation>Использовать имя проекта как название видео</translation>
    </message>
    <message>
        <source>Use the title defined in the project properties as the name of video</source>
        <translation>использование имени определено в свойствах проекта в качестве названия видео</translation>
    </message>
    <message>
        <source>Default device mode rendering options</source>
        <translation>Параметры создания видеофайла для различных устройств (по умолчанию)</translation>
    </message>
    <message>
        <source>Smartphone</source>
        <translation>Смартфон</translation>
    </message>
    <message>
        <source>Selects the default type and equipment model when you will click the &quot;Player and Smartphone&quot; button of the &quot;Render movie&quot; menu</source>
        <translation>Выберите вид оборудования, который будет предложен по умолчанию
при нажатии кнопки &quot;Плеер и смартфон&quot; в меню &quot;Создание видео&quot;</translation>
    </message>
    <message>
        <source>Multimedia system</source>
        <translation>Мультимедиа система</translation>
    </message>
    <message>
        <source>Selects the default type and equipment model when you will click the &quot;Multimedia system&quot; button of the &quot;Render movie&quot; menu</source>
        <translation>Выберите вид оборудования, который будет предложен по умолчанию
при нажатии кнопки &quot;Мультимедиа система&quot; в меню &quot;Создание видео&quot;</translation>
    </message>
    <message>
        <source>For the WEB</source>
        <translation>Интернет</translation>
    </message>
    <message>
        <source>Selects the default type and equipment model when you will click the &quot;For the Web&quot; button of the &quot;Render movie&quot; menu</source>
        <translation>Выберите вид оборудования, который будет предложен по умолчанию
при нажатии кнопки &quot;Интернет&quot; в меню &quot;Создание видео&quot;</translation>
    </message>
    <message>
        <source>Lossless</source>
        <translation>Сжатие без потерь</translation>
    </message>
    <message>
        <source>Selects the default image size to be used when you will click the &quot;Lossless&quot; button of the &quot;Render movie&quot; menu</source>
        <translation>Выберите формат изображения по умолчанию, который будет использоваться при нажатии кнопки Сжатие без потерь в меню &quot;Создание видео&quot;</translation>
    </message>
    <message>
        <source>Default advanced rendering options</source>
        <translation>Дополнительные параметры создания видеофайла (по умолчанию)</translation>
    </message>
    <message>
        <source>File format</source>
        <translation>Формат файла</translation>
    </message>
    <message>
        <source>Selects the default file type to use for the rendering out of the proposed list.</source>
        <translation>Выберите из выпадающего списка  формат создаваемого файла (по умолчанию).</translation>
    </message>
    <message>
        <source>Video standard</source>
        <translation>Стандарт видео</translation>
    </message>
    <message>
        <source>Selects the default video standard to use: PAL or NTSC.</source>
        <translation>Выберите используемый стандарт видео, PAL или NTSC.</translation>
    </message>
    <message>
        <source>Format</source>
        <translation>Формат</translation>
    </message>
    <message>
        <source>Selects the default image format to use for the rendering out of the proposed list.</source>
        <translation>Выберите из выпадающего списка формат изображения создаваемого видеофайла (по умолчанию).</translation>
    </message>
    <message>
        <source>Video codec</source>
        <translation>Видео кодек</translation>
    </message>
    <message>
        <source>Selects the default video codec to use for the rendering out of the proposed list.</source>
        <translation>Выберите из выпадающего списка видео кодек для создаваемого файла (по умолчанию).</translation>
    </message>
    <message>
        <source>Selects the default bit rate to use for video during the rendering out of the proposed list.</source>
        <translation>Выберите из выпадающего списка битрейт видео создаваемого файла (по умолчанию).</translation>
    </message>
    <message>
        <source>Audio codec</source>
        <translation>Аудио кодек</translation>
    </message>
    <message>
        <source>Selects the default audio codec to use for the rendering out of the proposed list.</source>
        <translation>Выберите из выпадающего списка аудио кодек для создаваемого файла (по умолчанию).</translation>
    </message>
    <message>
        <source>Selects the default bit rate to use for audio during the rendering out of the proposed list.</source>
        <translation>Выберите из выпадающего списка битрейт аудио создаваемого файла (по умолчанию).</translation>
    </message>
    <message>
        <source>Other actions</source>
        <translation>Другие действия</translation>
    </message>
    <message>
        <source>If the &lt;B&gt;Automatically crop 1920x1088 video to 1920x1080&lt;/B&gt; is checked then video images are automatically cropped. </source>
        <translation>Если установлен флажок &lt;B&gt;Автоматическая обрезка видео 1920x1088 до 1920x1080&lt;/B&gt;, то видеоизображение автоматически обрезается.</translation>
    </message>
    <message>
        <source>Remark: Some manufacturers produce FullHD videos 1920x1088 rather than 1920x1080. They make it to accelerate the encoding which is faster with a multiple of 16 lines. But this size is not 16:9. To keep the whole image, not only the image is deformed to fit into the screen but besides black bands appear on the sides. To avoid all these problems, by checking this box, these videos are truncated to be returned to the 1920x1080 sizes. Then 4 lines are cut at the top and bottom.</source>
        <translation>Примечание: некоторые производители создают FullHD видео с разрешением 1920 × 1088, а не 1920 × 1080. Они делают это для ускорения кодирования, которое происходит быстрее, если кратно 16 линиям. Но этот размер не 16:9. При вписании в экран изображения целиком, без деформации появляются черные полосы по бокам. Чтобы избежать всех этих проблем, установите этот флажок и такое видео будет обрезано до размера 1920 × 1080. По 4 строки обрезается сверху и снизу.</translation>
    </message>
    <message>
        <source>Remark: Some manufacturers produce videos using interlaced mode. They make it to accelerate the encoding. The problem is it deforms the image on which moved lines appear. For more information on interlaced video, see this Wikipedia page: </source>
        <translation>Примечание: некоторые производители выпускают видео, используя режим чересстрочной развертки. Они делают это, чтобы ускорить кодирование. Проблема в том, что он искажает изображение, на котором появляются линии движения. Для получения дополнительной информации по видео с чересстрочной разверткой, см. соответствующую статью в Википедии: </translation>
    </message>
    <message>
        <source>4:3: Corresponds to an old TV with (almost) square screen</source>
        <translation>4:3: соответствует старому телевизору с (почти) квадратным экраном</translation>
    </message>
    <message>
        <source>16:9: Corresponds to a modern TV with rectangular screen</source>
        <translation>16:9: соответствует современному телевизору с прямоугольным экраном</translation>
    </message>
    <message>
        <source>2.35:1: Corresponds to the Cinema format (on a TV, this format will use black strips at the top and below).</source>
        <translation>2.35:1: соответствует формату кино (на экране телевизора, это формат приведет к появлению черных полос вверху и внизу).</translation>
    </message>
    <message>
        <source>This group of settings applies to the new text blocks created in slide dialogue by means of this button: </source>
        <translation>Эта группа настроек применяется к новым текстовым блокам, созданным в окне редактирования слайда с помощью этой кнопки: </translation>
    </message>
    <message>
        <source>This group of settings applies to the new image or video blocks created from the main window by means of this button: </source>
        <translation>Эта группа настроек применяется к новому изображению или видео, созданным из главного окна с помощью этой кнопки: </translation>
    </message>
    <message>
        <source>Select, for each image format, the style of framing and the style of positioning to be applied to new blocks.</source>
        <translation>Выберите для каждого формата изображений стили кадрирования и позиционирования, применяемые к новым блокам.</translation>
    </message>
    <message>
        <source>This group of settings applies to the new image or video blocks created in slide dialogue by means of this button: </source>
        <translation>Эта группа настроек применяется к новому изображению или видео, созданным в окне редактирования слайда с помощью этой кнопки: </translation>
    </message>
    <message>
        <source>Bitrate</source>
        <translation>Битрейт</translation>
    </message>
    <message>
        <source>This dialog box allows you to define options for the application. It consists of the following 3 tabs:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The Application options tab allows you to define the general behavior of ffDiaporama.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The Project options tab allows you to define the options by default of the current project and for the new projects.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The Rendering options Tab allows you to define the options used by default during the generation of videos.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Disable tooltips in dialog box</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If this box is checked, tooltips will no longer appear in dialog boxes (only what&apos;s this will be available).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Offer to open the download Web page when a new version is available</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If this box is checked, a dialog box will appear at startup the next time a new version will be available. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>By default, this box is checked for Windows users and unchecked for Linux users</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the quantity of memory used as cache memory by ffDiaporama. This parameter is important because it allows to adapt ffDiaporama to your hardware. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>For users of 32-bit systems, it is recommended that set this value to 512 MB if the computer has at least 2 GB of RAM. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>For users of 64-bit systems, it is recommended that set this value to 2 GB if the computer has at least 4 GB of RAM.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Max video preview height</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Sets the maximum height of videos used for the preview. Beyond this height, the videos are automatically reduced for creating images. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This parameter affects only the preview and does not change the quality of rendered video. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>It is recommended to reduce this value for slow computer.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This effect consumes lots of resources and can result in slow preview on small machines. Unchecking this box allows then to keep a fluid preview. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This parameter affects only the preview and does not change the quality of rendered video.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Sound sample rate</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the frequency of the sound generated to show the preview. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Video cropping</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Video deinterlacing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Various options</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Short date format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This option allows you to specify the format in which computed dates are displayed. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Normally this format depends on the language used on your system, but it can be useful to change it especially if the interface is not available for your language.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the default value for the field Author that is often so called Artist by </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Media Center Systems</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Enhance id3v2 comptatibility</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>For more compatibility with the standard id3v2, check this box to limit Title, Author and Album fields to 30 characters</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Default thumbnail</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the model use as default thumbnail. Thumbnails are images that are generated along with the videos. These images are used as a poster by most </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>For more information on joined files, see &lt;a href=&quot;003A.html&quot;&gt;File attachments&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows to select the style of the shape to applied to the new text blocks. This style defines the shape (rectangle, circle, ...), the border, the opaqueness and the shadow effect applied to the block.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Default joined files options</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Join thumbnail to rendered file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If this box is checked then a thumbnail file will be created with video.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Join XBMC .nfo file to rendered file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If this box is checked then a XMBC .nfo file will be created with video.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Frequency</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Select the default frequency to use for audio during the rendering.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Default soundtrack rendering options</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>At the bottom of the dialog these buttons allow you to:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Check configuration allows to open the &lt;a href=&quot;0106.html&quot;&gt;Check configuration dialog&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Manage device allows to open the &lt;a href=&quot;0102.html&quot;&gt;Manage device dialog&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0102</name>
    <message>
        <source>Manage devices dialog</source>
        <translation>Диаголовое окно Управление устройствами</translation>
    </message>
    <message>
        <source>The default database (supplied with ffDiaporama). These devices are registered in the global ffDiaporama configuration file.</source>
        <translation>База данных по умолчанию (поставляется с ffDiaporama). Эти устройства, зарегистрированы в глобальном файле конфигурации ffDiaporama.</translation>
    </message>
    <message>
        <source>The devices which you modify or add. These are registered in your personal configuration file.</source>
        <translation>Устройства, которые ви изменили или добавили. Они регистрируются в вашем личном файле конфигурации.</translation>
    </message>
    <message>
        <source>The database is actually composed of two parts: </source>
        <translation>База данных устройств состоит из двух частей:</translation>
    </message>
    <message>
        <source>You must first create a device</source>
        <translation>Сперва вы должны создать устройство</translation>
    </message>
    <message>
        <source>You define the various parameters</source>
        <translation>Затем задать различные  параметры обработки</translation>
    </message>
    <message>
        <source>You click the &quot;Apply modifications&quot; button to register the parameters</source>
        <translation>Нажать кнопку &quot;Применить изменения&quot; для регистрации параметров</translation>
    </message>
    <message>
        <source>Remarks:</source>
        <translation>Примечания:</translation>
    </message>
    <message>
        <source>Device subtype</source>
        <translation>Подтип устройства</translation>
    </message>
    <message>
        <source>Allows you to select the category in which the equipment is classified</source>
        <translation>Позволяет выбрать категорию, в которой позиционируется оборудование</translation>
    </message>
    <message>
        <source>Device model</source>
        <translation>Модель устройства</translation>
    </message>
    <message>
        <source>Note that you can delete only devices which you created - those shown with this icon: </source>
        <translation>Обратите внимание, что удалить можно только устройства, созданные вами. Они помечены значком: </translation>
    </message>
    <message>
        <source>Enter here the name of the device (free text entry field) and then, enter all the parameters required for rendering videos that are to be associated with the device profile (File format, video Standard, etc.)</source>
        <translation>Введите здесь имя устройства (обычное текстовое поле) и укажите все параметры, необходимые для создания видео, которые должны быть связаны с профилем устройства (формат файла, стандарт видео, и т.д.)</translation>
    </message>
    <message>
        <source> This dialog box allows you to modify the equipment profiles database. </source>
        <translation>Это диалоговое окно позволяет изменять базу данных профилей оборудования.</translation>
    </message>
    <message>
        <source>Devices preceded by this icon are defined in the default database.</source>
        <translation>Устройства, помеченные этим значком, внесены в базу данных по умолчанию.</translation>
    </message>
    <message>
        <source>Devices preceded by this icon are those that you defined yourself, or those from the default database that you have modified.</source>
        <translation>Устройства, помеченные этим значком, созданы или изменены вами.</translation>
    </message>
    <message>
        <source>Add device: Allows you to create a new device which will be added to the database. </source>
        <translation>Добавить устройство: позволяет создать новое устройство, которое будет добавлено в базу данных. </translation>
    </message>
    <message>
        <source>The creation of a new device is made in 3 steps: </source>
        <translation>Создание нового устройства производится в 3 шага: </translation>
    </message>
    <message>
        <source>Remove device: Allows you to delete a device. </source>
        <translation>Удалить устройство: позволяет удалить устройство. </translation>
    </message>
    <message>
        <source>Reset to default: Cancels the modifications that you made on a device defined in the default ffDiaporama database</source>
        <translation>По умолчанию: отменяет изменения, сделанные с устройствами базы данных ffDiaporama по умолчанию</translation>
    </message>
    <message>
        <source>The device type zone allows you to select the type of equipment to be shown in the list.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The list shows the categories and the device types present in the database.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The following buttons allow you to act on the database:</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0103</name>
    <message>
        <source>Add or modify a predefined title slide</source>
        <translation>Добавление или изменение слайда из шаблона</translation>
    </message>
    <message>
        <source>This dialog allows you to select a model and define chapter information for this predefined slide.</source>
        <translation>Это диалоговое окно позволяет выбрать шаблон и задать информацию о главе для предустановленного слайда.</translation>
    </message>
</context>
<context>
    <name>0104</name>
    <message>
        <source>Background properties</source>
        <translation>Свойства фона</translation>
    </message>
    <message>
        <source>Same background as preceding object.</source>
        <translation>Тот же фон, что и у предыдущего объекта.</translation>
    </message>
    <message>
        <source> If this option is selected, the slide will have no background definition  but will use the same background as the previous slide: </source>
        <translation>Если этот параметр выбран, настройка фона слайда будет недоступна и фон будет как у предыдущего слайда: </translation>
    </message>
    <message>
        <source>If the previous slide does not have a background definition either, the check continues backwards until a slide is found which does have a background definition.</source>
        <translation>если предыдущий слайд не имеет фона, то будет найден первый предыдущий слайд, имеющий фон.</translation>
    </message>
    <message>
        <source>If no previous slide has a background definition, then a black frame will be used.</source>
        <translation>если нет предыдущего слайда, имеющего фон, то будет использоваться черная рамка.</translation>
    </message>
    <message>
        <source>Select a new background</source>
        <translation>Выбор нового фона</translation>
    </message>
    <message>
        <source> If this option is selected, the slide will have its own background definition. In that case, the following zones set properties for the background. </source>
        <translation>Если выбрана эта опция, слайд будет иметь свой собственный фон. В этом случае будут доступны следующие параметры настройки фона. </translation>
    </message>
    <message>
        <source>Solid brush</source>
        <translation>Твердая кисть</translation>
    </message>
    <message>
        <source>Selects a plain color.</source>
        <translation>позволяет выбрать сплошной цвет.</translation>
    </message>
    <message>
        <source>Gradient 2 colors</source>
        <translation>Градиент из двух цветов</translation>
    </message>
    <message>
        <source>Selects a gradient of 2 colors</source>
        <translation>позволяет выбрать градиент, состоящий из 2 цветов</translation>
    </message>
    <message>
        <source>Gradient 3 colors</source>
        <translation>Градиент из трех цветов</translation>
    </message>
    <message>
        <source>Selects a gradient of 3 colors</source>
        <translation>позволяет выбрать градиент, состоящий из 3 цветов</translation>
    </message>
    <message>
        <source>Image from library</source>
        <translation>Изображение из библиотеки</translation>
    </message>
    <message>
        <source>Image disk</source>
        <translation>Изображение с диска</translation>
    </message>
    <message>
        <source>Allows you to select a photo or an image. </source>
        <translation>позволяет выбрать фотографию или изображение из файла на диске.</translation>
    </message>
    <message>
        <source>File</source>
        <translation>Файл</translation>
    </message>
    <message>
        <source>Indicates the name of the current file</source>
        <translation>Указывает имя текущего файла</translation>
    </message>
    <message>
        <source>Keep aspect ratio</source>
        <translation>Сохранить пропорции</translation>
    </message>
    <message>
        <source>Full filling</source>
        <translation>Растянуть</translation>
    </message>
    <message>
        <source>Allows the image to fill the whole screen with image deformation as needed.</source>
        <translation>Позволяет растянуть изображение  на весь экран, даже если будет при этом деформировано.</translation>
    </message>
    <message>
        <source>Allow you to selects an image file</source>
        <translation>Позволяет выбрать файл с изображением</translation>
    </message>
    <message>
        <source>If this box is checked, then the image will not be deformed, and: </source>
        <translation>Если этот флажок установлен, изображение не деформируется и: </translation>
    </message>
    <message>
        <source> Open the &lt;a href=&quot;0110.html&quot;&gt;Image framing and correction dialog&lt;/a&gt; to regulate framing and image corrections </source>
        <translation>открывается диалоговое окно &lt;a href=&quot;0110.html&quot;&gt;Кадрирование и коррекция изображения&lt;/a&gt; для обрезки и коррекции изображения </translation>
    </message>
    <message>
        <source>Selects a background image from the library </source>
        <translation>позволяет выбрать предопределенный фон </translation>
    </message>
    <message>
        <source>Remark: By default, the library contains a dozen of background screens. </source>
        <translation>Примечание: по умолчанию библиотека содержит около десятка фоновых изображений. </translation>
    </message>
    <message>
        <source>Download extensions</source>
        <translation>Страница загрузки расширений</translation>
    </message>
    <message>
        <source>You can get additionals backgrounds installing the &lt;B&gt;&lt;U&gt;Texturemate&lt;/U&gt;&lt;/B&gt; extension from this page </source>
        <translation>Вы можете увеличить количество фоновых изображений, установив расширение &lt;B&gt;&lt;U&gt;Texturemate&lt;/U&gt;&lt;/B&gt; с этой страницы </translation>
    </message>
    <message>
        <source>The &quot;Background properties&quot; dialog allows you to define settings for the background of the slide.</source>
        <translation>Диалоговое окно &quot;Свойства фона&quot; позволяет определить параметры фона слайдов.</translation>
    </message>
</context>
<context>
    <name>0105</name>
    <message>
        <source>Chapter properties</source>
        <translation>Свойства главы</translation>
    </message>
    <message>
        <source>This dialog allows you to define settings for the chapter information of the slide</source>
        <translation>Это диалоговое окно позволяет задать настройки информации о главе</translation>
    </message>
</context>
<context>
    <name>0106</name>
    <message>
        <source>Check configuration</source>
        <translation>Проверить конфигурацию</translation>
    </message>
    <message>
        <source>It is especially useful for the Linux users and for those who compiles the application. </source>
        <translation>Это особенно полезно для пользователей Linux и тех, кто компилирует приложение. </translation>
    </message>
    <message>
        <source>The most common shown errors concern the supported formats, codec and library versions. </source>
        <translation>Показаны наиболее распространенные ошибки, касающиеся поддерживаемых форматов, версий кодеков и библиотек.</translation>
    </message>
    <message>
        <source>This dialog allows to check external tools configuration and libraries used by ffDiaporama.</source>
        <translation>Это диалоговое окно позволяет проверить конфигурацию внешних инструментов и библиотек, используемых ffDiaporama.</translation>
    </message>
    <message>
        <source>To solve the problems you will have to install additional packages on your computer and may be to make ffDiaporama from source and verify again the configuration. </source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0107</name>
    <message>
        <source>Export project</source>
        <translation type="unfinished">Экспорт проекта</translation>
    </message>
    <message>
        <source>This dialog box allows you to export the project and all ressources used by the project to a folder</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0108</name>
    <message>
        <source>Project properties</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This dialog allows you to define the information which qualifies the project. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This information will then be registered in the form of TAGs in the rendered video files. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Some fields are automatically filled and do not appear in this dialog: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remarks:</source>
        <translation type="unfinished">Примечания:</translation>
    </message>
    <message>
        <source>The Project tab</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Settable fields</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Fields</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remark</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Project geometry</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This field allows you to select the geometry to be used for the project. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Title</source>
        <translation type="unfinished">Название</translation>
    </message>
    <message>
        <source>Author</source>
        <translation type="unfinished">Автор</translation>
    </message>
    <message>
        <source>Album</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Language</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Comment</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A free comment which can spread out on several lines. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Timestamp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Date and time of generation of the video.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Composer</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Contains the version of ffDiaporama with which the video was produced.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Encoder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Contains the version of libavformat (ffmpeg) with which the video was produced.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Chapters</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The following fields appear: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>#</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Chapter number</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Slide</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Slide number in wich chapter is defined</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Chapter title</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Start</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Chapter start position</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>End</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Chapter end position</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Duration</source>
        <translation type="unfinished">Продолжительность</translation>
    </message>
    <message>
        <source>Chapter duration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Format</source>
        <translation type="unfinished">Формат</translation>
    </message>
    <message>
        <source>3GP/MP4/MKV</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Full support</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>AVI/MPG/FLV/OGV/WEBM</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>No support</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The option tab</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Options for speed waves</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> These options sets the value of the velocity curves for use whenever the corresponding options are set to &quot;Project Default&quot; </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Transition</source>
        <translation type="unfinished">Переход</translation>
    </message>
    <message>
        <source>Defined default speed curve for transitions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Block animations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defined default speed curve for block animations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Image animations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defined default speed curve for image animations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>See also</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>TAG in ffmpeg</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Standard ID3v2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Standard MPEG-7 on the descriptors of multimedia contents</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The properties of the project are a partial implementation of the </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>MPEG-7</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> standard on the descriptors of multimedia contents.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>These properties are written into the video files during the rendering and can be exploited by indexing systems (collection managers) or by </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Media Center Systems</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remark:</source>
        <translation type="unfinished">Примечание:</translation>
    </message>
    <message>
        <source>This field is only editable when the dialog box is used for creating a new project. Subsequently, it is not possible to change this value.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The title of the project </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If the id3v2 compatibility option is set in &lt;a href=&quot;0101.html&quot;&gt;option dialog&lt;/a&gt;, this fields is limited to 30 characters</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Event date</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The date of the event of the project</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Override date</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Check this box to override the date and offer you to enter the date as a free multi-line text</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The name of the author of the project. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This field is often shown by indexing system and </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> under the term Artist.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The album of the project </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The language of the audio track </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>(ISO 639 standard - 3 characters)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Numerous indexing systems or </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> will read only the 1&lt;SUP&gt;st&lt;/SUP&gt; line.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>For more compatibility with indexing systems and </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>, ffDiaporama does not include the whole video TAGs but is closer rather to the standard id3v2 than is the most used by these systems today. Furthermore, the video formats do not support every TAG in the same way.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Chapters defined in the project are listed for consultation. (To modify chapters use Chapter button in &lt;a href=&quot;0119.html&quot;&gt;modify slide dialog&lt;/a&gt;.) </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remark: Video formats do not support all the chapters in the same way:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Options for joined ressources</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> These options allow you to define the thumbnail that will be attached to the video file when rendering </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Thumbnail</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the model use as default thumbnail. The combobox contains the available thumbnails: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> If this icon appears, then the thumbnail is a custom model thumbnail (Ie, a custom model thumbnail that you have added to your collection of thumbnail model).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> If this icon appears, then the thumbnail is a custom thumbnail for this project only (Ie, not based on a model).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Edit custom thumbnail</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Create a new model from this thumbnail or edit custom thumbnail. Opens the &lt;a href=&quot;0117.html&quot;&gt;Edit thumbnail dialog&lt;/a&gt;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Customize</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Customize the thumbnail only for this project (without creating a new thumbnail model). Opens the &lt;a href=&quot;0117.html&quot;&gt;Edit thumbnail dialog&lt;/a&gt;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Export thumbnail</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Expor thumbnail to a jpeg file. By default, the proposed file name is &quot;folder.jpg&quot;. ffDiaporama and many </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> recognize this file as the icon to be used for the directory that contains it.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>ISO 639</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0109</name>
    <message>
        <source>Select files</source>
        <translation type="unfinished">Выбор файлов</translation>
    </message>
</context>
<context>
    <name>0110</name>
    <message>
        <source>Correct, reframe or cut image or video</source>
        <translation type="unfinished">Коррекция, кадрирование или обрезка изображения или видео</translation>
    </message>
    <message>
        <source>Allows to select a framing from a list of predefined framings. The framings are classified in different categories: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Lock on the project geometry</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Lock the image framing selection rectangle to the same geometry as the project. If you place the image in full screen, the reframed part will correspond to the whole screen.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Lock on the image geometry</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Lock the image framing selection rectangle to the same geometry as the image</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>No constraint</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>No constraint is applied to the image framing selection rectangle.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Lock on this geometry</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Lock image framing selection rectangle to the actual geometry. This only has meaning when a shape has been defined in &quot;no constraint&quot; mode.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>According to the image format, various choices will be proposed to you. For example: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The horizontal position from the left (X). This position is expressed as a percentage (%) of the width of the image.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The vertical position from the top of the image ( Y ). This position is expressed as a percentage (%) of the height of the image.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The width. This value is expressed as a percentage (%) of the original width of the image.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The height. This value is expressed as a percentage (%) of the original height of the image.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remarks:</source>
        <translation type="unfinished">Примечания:</translation>
    </message>
    <message>
        <source>With the mouse</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Click one of the handles and, with the mouse button down, drag the mouse to modify the size (to increase or reduce the part of the image to be kept). </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Click the image and, with the mouse button down, drag the mouse to modify the part of the image which will be kept.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>With the keyboard</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Use the arrows to move the part of the image which will be kept. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Use SHIFT+arrows to move the upper left corner handle or CTRL+arraows to move the lower right corner handle, to change the size of the part of the image which will be kept.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>When the image does not correspond to the geometry of the video to be produced, transparent zones appear. For example: an 4:3 image used in a 16:9 project will be shown by default with a transparent zone on each side.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to increase or decrease the luminosity of the image</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to increase or decrease the contrast of the image</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to to increase or decrease the gamma value of the image</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to modify the colorization of an image by changing the primary color red. Example : Reducing the red component makes the image more yellow - increasing it makes make the image more red</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to modify the colorization of an image by changing the primary color green. Example : Reducing the green component makes the image more red - increasing it makes the image more green</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to modify the colorization of an image by changing the primary color blue. Example : Reducing the green component makes the image more red - increasing it makes the image more blue</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This button allows the value to be reset to zero for all correction settings.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Increase the blur with a negative value, using radius.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Increase the sharpen with a positive value, using radius.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The progress bar</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Displays as follows:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The length of the ruler is increased in proportion to the duration of the video.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The blue part represents the part of the video which is selected (which will be used in the assembly).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The working range</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Positions the cursor at the starting point</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Uses the current cursor position as starting point (carries out an edit)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Positions the cursor at the end point</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Uses the current cursor position as end point (carries out an edit)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The additional settings zone</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;Volume&lt;/B&gt; allows you to define the &quot;Master Volume&quot; for the file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;Deinterlace video&lt;/B&gt; allows you to activate the deinterlace filter</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>See also</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Ken Burns effects</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Filter: Histogram equalization</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Filter: Noise reduction</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>You can use the + and - buttons in each input field to change the settings, or directly enter values for the four parameters.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Example: if X=25% then the 1st quarter to the left of the image is eliminated.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Example: if Y=25% the 1st quarter at the top of the image is eliminated.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Example: if Width=33%, the re-framed image will represent 1/3 of the original.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Example: if Height=33%, the re-framed image will represent 1/3 of the original.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This button allows you to change the file </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Warning! If you replace a file (image or video) with another one with a different image geometry, you will need to review the image framing in all shots. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Blur/Sharpen </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This button allows you to reset the value to zero. (For some transformation settings.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This button starts playback. Click again on this button to pause. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This dialog box allows you to:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Change image framing (select a part of the image).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Rotate image (change or restore the orientation portrait/landscape).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Correct image (change the color and/or exposure parameters) but also applying graphical filters.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Define the beginning and end of a video sequence.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> The image correct and reframe tag</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This tab is available for both images and video. It is divided into two parts:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To the left: The interactive area</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To the right: The settings area</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>At the bottom of the interactive zone, is an area for changing the image or video file:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>At the bottom of the dialog box:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This button enables or disables the magnetic rulers: When rulers are enabled, the mouse will automatically snap to the edges of the photo, facilitating the framing. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The interactive area</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>You can use the interactive zone to set the framing with the mouse or the keyboard:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The settings area - Framing and form</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shape</source>
        <translation type="unfinished">Форма</translation>
    </message>
    <message>
        <source>Select a shape for the current block.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The settings area - Annimation settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Speed wave</source>
        <translation type="unfinished">Скорость</translation>
    </message>
    <message>
        <source>Defined speed curve for image animations.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The settings area - Image rotation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The rotation angle for the z axis. This position is expressed in degrees. Example: if Rotation=90Â°, the image is turned by a quarter to the right.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Turn the image to the left (to the nearest multiple of 90Â°).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Turn the image to the right (to the nearest multiple of 90Â°).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The settings area - Luminosity, contrast and gamma and Color enhancement</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to activate the grayscale (black &amp; white) filter.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to activate the equalize (Histogram equalization) filter.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>All the combinations are possible. For example: you can increase the red and reduce the green at the same time. The results depend on the initial strength of the primary color components of the source image.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The settings area - Image enhancement</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Apply a filter to the photo or video images. The following filters are available: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Type: Select the blur/sharpen algorythm</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to activate the despeckle (Noise reduction) filter.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to activate the antialias (minimizing the distortion artifacts) filter.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The settings area - Artistic effects</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to activate the negative filter..</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to activate the emboss filter.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to activate the edge filter.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to activate the charcoal filter.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to activate the oilpaint filter.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The settings area - Image distortion</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to activate the swirl transformation. Image swirling is a non-linear image deformation that creates a whirlpool effect.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to activate the implode transformation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A value of 0 disables the filter.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A negative or positive value active the filter in one direction or the other.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This button allows the value to be reset to zero.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> The video tag</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This tag is available only for video.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0111</name>
    <message>
        <source>File information</source>
        <translation type="unfinished">Информация о файле</translation>
    </message>
    <message>
        <source>Information shown for the image files</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Information on file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>File name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The name of the file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>File type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Image</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>File size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The size of the file on the disk (Mb)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>File created</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The date and the hour of creation of the file on the disk (when you transferred it since the camera)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>File modify</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The date and the hour of the last modification of the file (when the photo was taken or when you modified it with an image editor)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Additional information</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Show the EXIF information supplied by the camera. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This zone can contain several hundreds of lines according to what the camera supplies. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Information shown for the audio and video files</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Information on file (audio and video files)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Audio track information (audio and video files)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This zone contains a table listing every audio track present in the file. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>For every track, the following information is shown: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>#</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Track number</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Language</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The language in which is the audio track (generally cameras and camcorders indicate &lt;B&gt;und&lt;/B&gt; for undefine)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Codec</source>
        <translation type="unfinished">Кодек</translation>
    </message>
    <message>
        <source>The audio codec used for compressed the sound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Channels</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The number of audio channels (1=mono, 2=stereo, etc.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Bitrate</source>
        <translation type="unfinished">Битрейт</translation>
    </message>
    <message>
        <source>The audio compression ratio (when available)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Frequency</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The frequency of sampling used by the audio track</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Title</source>
        <translation type="unfinished">Название</translation>
    </message>
    <message>
        <source>A title (when available)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Video track information (video files only)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This zone contains a board listing every video track present in the file. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Image size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The size of the images of the video track in number of points by number of line</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Image format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The size of the images of the video track in million of pixels</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Image geometry</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The image geometry of the video track (4:3, 16:9, etc.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The video codec used for compressed the images</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Frame rate</source>
        <translation type="unfinished">Частота кадров</translation>
    </message>
    <message>
        <source>The number of images per second of the video</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The video compression ratio (when available)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Chapter information (video files only)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This zone appears only for files containing chapters. it contains a table listing every chapter present in the file. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>For every chapter, the following information is shown: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Chapter number</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The title of the chapter</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Start</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The temporal position of the beginning of the chapter</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>End</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The temporal position of the end of the chapter</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Duration</source>
        <translation type="unfinished">Продолжительность</translation>
    </message>
    <message>
        <source>The duration of the chapter</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Additional information (video files only)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This zone contains diverse information on the video. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>At least the information about the duration of the video will be shown, but a lot of other information can appear according to what is available in the file, such as: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>creation_time</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Date and time of creation of the video</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>title</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Title field of the TAG ID3v2 of the video</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>artist</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Artist field of the TAG ID3v2 of the video</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>album</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Album field of the TAG ID3v2 of the video</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>date</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Date field of the TAG ID3v2 of the video</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>comment</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Comment field of the TAG ID3v2 of the video</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>composer</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The software with which the video was created</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>See also</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Information on EXIF data</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>TAG in ffmpeg</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Standard ID3v2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Standard MPEG-7 on the descriptors of multimedia contents</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This dialog allows to obtain detailed information on a multimedia file.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0112</name>
    <message>
        <source> Rename: Allows to rename the selected favorite.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Remove: Allows to remove the selected favorite.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Manage favorites</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The Manage favorite dialog allows you to manage existing favorites</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The existing favorites are listed in the high part of the dialog. Select one of them and click on one of the following buttons:</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0113</name>
    <message>
        <source>Manage style</source>
        <translation>Управление стилями</translation>
    </message>
    <message>
        <source>Reset a standard style (that you modified) has its default values.</source>
        <translation>Сброс настроек стандартного стиля (которые были изменены) на значения по умолчанию.</translation>
    </message>
    <message>
        <source>Standard style</source>
        <translation>Стандартный стиль</translation>
    </message>
    <message>
        <source>Rename</source>
        <translation>Переименовать</translation>
    </message>
    <message>
        <source>Standard style you modified</source>
        <translation>Стандартный стиль, который вы изменили</translation>
    </message>
    <message>
        <source>Rename, Reset to default</source>
        <translation>Переименовать, Сбросить по умолчанию</translation>
    </message>
    <message>
        <source>Custom style</source>
        <translation>Пользовательский стиль</translation>
    </message>
    <message>
        <source>Rename, Remove</source>
        <translation>Переименовать, Удалить</translation>
    </message>
    <message>
        <source>Allows to rename the selected style.</source>
        <translation>Позволяет переименовать выделенный стиль.</translation>
    </message>
    <message>
        <source>Reset to default</source>
        <translation>Сбросить по умолчанию</translation>
    </message>
    <message>
        <source>Allows to delete the customizations of a standard style and thus to return it to the default values for this style. It concerns as well the name of the style as the associated settings.</source>
        <translation>Позволяет удалить настройки стандартного стиля, тем самым вернув его значения по умолчанию. Это касается как имени стиля так и соответствующих настроек.</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Удалить</translation>
    </message>
    <message>
        <source>Allows to remove the personalized style.</source>
        <translation>Позволяет удалить пользовательский стиль.</translation>
    </message>
    <message>
        <source>Rename existing style,</source>
        <translation>Переименовать существующий стиль,</translation>
    </message>
    <message>
        <source>Remove custom style,</source>
        <translation>Удалить пользовательский стиль,</translation>
    </message>
    <message>
        <source>for a standard style</source>
        <translation>для стандартного стиля</translation>
    </message>
    <message>
        <source>for a standard style you modified or for a custom style</source>
        <translation>для стандартного стиля, который вы изменили, или для пользовательского стиля</translation>
    </message>
    <message>
        <source>The existing styles are listed in the high part of the dialog. They appear with an icon which has various meanings:</source>
        <translation>Список существующих стилей представлен в верхней части диалогового окна. Они появляются с значком, который имеет разные значения:</translation>
    </message>
    <message>
        <source>The Manage style dialog allows several things:</source>
        <translation>Диалоговое окно Управление стилями позволяет сделать несколько вещей:</translation>
    </message>
    <message>
        <source>According to the type of the selected style, you can execute various actions:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The followed actions are available:</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0114</name>
    <message>
        <source>Music properties</source>
        <translation>Параметры музыки</translation>
    </message>
    <message>
        <source>Settings for the music track (upper part of the dialog)</source>
        <translation>Параметры музыкального трека (верхняя часть окна)</translation>
    </message>
    <message>
        <source>Functional Principles</source>
        <translation>Принципы функционирования</translation>
    </message>
    <message>
        <source> The main functional principle for music tracks is that as long as you do not change the settings, the previous settings apply. So, if: </source>
        <translation>Основным принципом функционирования музыкальных дорожек является то, что до тех пор пока вы не измените настройки применяются предудыщие параметры. Так, если:</translation>
    </message>
    <message>
        <source>Slide 1 - Defines a play list</source>
        <translation>Слайд 1 - определен плейлист</translation>
    </message>
    <message>
        <source>Slide 2 - Contains no settings</source>
        <translation>Слайд 2 - не содержит настроек музыки</translation>
    </message>
    <message>
        <source>Slide 3 - Contains no settings</source>
        <translation>Слайд 3 - не содержит настроек музыки</translation>
    </message>
    <message>
        <source>Slide 4 - Defines a play list</source>
        <translation>Слайд 4 - определен плейлист</translation>
    </message>
    <message>
        <source>Slide 5 - Contains no settings</source>
        <translation>Слайд 5 - не содержит настроек музыки</translation>
    </message>
    <message>
        <source>Slide 2 and 3 continue to play the play list defined in slide 1</source>
        <translation>На слайдах 2 и 3 продолжает воспроизводиться плейлист, определенный на слайде 1</translation>
    </message>
    <message>
        <source>Slide 5 continues to play the play list define in slide 4</source>
        <translation>На слайде 5 продолжает воспроизводиться плейлист, определенный на слайде 4</translation>
    </message>
    <message>
        <source>Available settings for the music track</source>
        <translation>Доступны следующие настройки музыкального трека</translation>
    </message>
    <message>
        <source>Continue current playlist</source>
        <translation>Продолжать проигрывать текущий плейлист</translation>
    </message>
    <message>
        <source>Continues to play the play list defined by a previous slide:</source>
        <translation>Продолжить плейлист, определенный в предыдущем слайде:</translation>
    </message>
    <message>
        <source>Normal</source>
        <translation>Без изменений</translation>
    </message>
    <message>
        <source>No specific settings</source>
        <translation>Без пользовательских настроек</translation>
    </message>
    <message>
        <source>Change sound level</source>
        <translation>Изменить уровень громкости</translation>
    </message>
    <message>
        <source>Specifies that from now on, the sound volume is reduced. This option is particularly useful, for example, to reduce the sound of the music during the play of a video. Remark : This volume reduction is applied only for the selected slide.</source>
        <translation>Указывает, что с этого момента, громкость звука уменьшается. Эта настройка особенно полезна, например, для уменьшения звука музыки во время воспроизведения видео. Примечание: заданная громкость применяется только для выбранного слайда.</translation>
    </message>
    <message>
        <source>Set to pause</source>
        <translation>Поставить на паузу</translation>
    </message>
    <message>
        <source>Pauses the current play list. Remark : This pause is applied only for the selected slide.</source>
        <translation>Пауза текущего списка воспроизведения. Примечание: эта пауза применяется только для выбранного слайда.</translation>
    </message>
    <message>
        <source>Start a new playlist</source>
        <translation>Начать новый плейлист</translation>
    </message>
    <message>
        <source>Allows you to create a new playlist. If a list defined in a previous slide, is still playing, it is stopped with a fade-out effect (progressive decline of the sound volume up to complete stop).</source>
        <translation>Позволяет создать новый плейлист. Если список, определенный в предыдущем слайде, до сих пор играет, то он останавливается с эффектом затухания (прогрессивное снижение громкости звука до полной остановки).</translation>
    </message>
    <message>
        <source>The playlist (bottom part of the dialog)</source>
        <translation>Список воспроизведения (нижняя часть окна)</translation>
    </message>
    <message>
        <source> This playlist section consists of two parts: </source>
        <translation>Список воспроизведения состоит из двух частей: </translation>
    </message>
    <message>
        <source>The toolbar containing controls for the playlist</source>
        <translation>Панель инструментов, содержащая элементы управления списком воспроизведения</translation>
    </message>
    <message>
        <source>The list of the music files composing the playlist</source>
        <translation>Список музыкальных файлов, составляющих в плейлист</translation>
    </message>
    <message>
        <source>The toolbar</source>
        <translation>Панель инструментов</translation>
    </message>
    <message>
        <source>Adds a file to the playlist A file selection dialog appears, allowing you to select a file.</source>
        <translation>Добавляет файл в список воспроизведения. Появится диалоговое окно выбора файлов, позволяющее выбрать файл.</translation>
    </message>
    <message>
        <source>Removes the current selected file from the playlist.</source>
        <translation>Удаляет выбранный файл из списка воспроизведения.</translation>
    </message>
    <message>
        <source>Changes the order in which the music files will be played. Moves the currently selected file one step up.</source>
        <translation>Изменение порядка в котором музыкальные файлы будут воспроизводиться. Перемещение выбранного файла на один шаг вверх.</translation>
    </message>
    <message>
        <source>Changes the order in which the music files will be played. Moves the currently selected file one step down.</source>
        <translation>Изменение порядка в котором музыкальные файлы будут воспроизводиться.
Перемещение выбранного файла на один шаг вниз.</translation>
    </message>
    <message>
        <source>The playlist</source>
        <translation>Список воспроизведения</translation>
    </message>
    <message>
        <source>The music files are listed in the order in which they will be played, from top to bottom. The Volume column defines the &quot; Master volume &quot; for the file. This volume is independent of any reductions in the volume that you define in the upper part of the dialog.</source>
        <translation>Музыкальные файлы перечислены в том порядке, в котором они будут воспроизводиться, сверху в низ. Столбец Громкость определяет уровень громкости файла. Эта громкость не зависит от уменьшения громкости, которую вы задали в верхней части диалогового окна.</translation>
    </message>
    <message>
        <source>Remarks:</source>
        <translation>Примечания:</translation>
    </message>
    <message>
        <source>The fade in/out effects and the increase/decrease of volume take place during the entering slide transitions. Consequently: </source>
        <translation>Эффекты затухания / нарастания  и увеличение / уменьшение громкости происходят во время входного перехода между слайдами. Следовательно: </translation>
    </message>
    <message>
        <source>They last for the same time as the entering transition.</source>
        <translation>Они происходят за то же время, что и входящий переход.</translation>
    </message>
    <message>
        <source>If a slide has an entering transition set to &quot;No transition&quot;, the sound effect is not progressive but instantaneous.</source>
        <translation>Если слайд имеет входящий переход типа &quot;Без перехода&quot;, то звуковой эффект является не прогрессивным, а мгновенным.</translation>
    </message>
    <message>
        <source>The Music properties dialog allows you to specify the music track.</source>
        <translation>Диалоговое окно Параметры музыки позволяет настроить музыкальный трек.</translation>
    </message>
    <message>
        <source>Then:</source>
        <translation>Тогда:</translation>
    </message>
</context>
<context>
    <name>0115</name>
    <message>
        <source>Render video</source>
        <translation type="unfinished">Создание видео</translation>
    </message>
    <message>
        <source>Description of the dialog box</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Destination file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Indicate here the name of the destination file. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Click on this button to select graphically a file and a directory.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Rendering</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The rendering can be made on the whole project or only on a part.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This choice is made: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>By checking the option &lt;B&gt;All slides&lt;/B&gt; to render the video of the whole project</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Or by checking the option &lt;B&gt;From&lt;/B&gt; and then entering the numbers of the start and end slides to generate a video of only a part of the project.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Sound (include sound in the video)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If this box is checked then the sound track will be included in the video</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Language</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the value of the Language field. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>In device mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Device type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to select a device type.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Model</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to select a device model.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>In lossless mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Image format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Select the image format to be used to render the video from the displayed list.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>In advanced mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>File format</source>
        <translation type="unfinished">Формат файла</translation>
    </message>
    <message>
        <source>Select the type of file to be used to render the video from the displayed list.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Video Standard</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Select the video standard to be used: PAL or NTSC.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Video codec</source>
        <translation type="unfinished">Видео кодек</translation>
    </message>
    <message>
        <source>Select the video codec to be used to render the video from the displayed list.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Select the video compression ratio from the displayed list</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Audio codec</source>
        <translation type="unfinished">Аудио кодек</translation>
    </message>
    <message>
        <source>Select the audio codec to be used to render the video from the displayed list.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Render the movie</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Once you have selected the device or set the advanced parameters, one click on the OK button starts the process of rendering the video. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>At any time during this process, one click on the &quot;Cancel&quot; button stops the process. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remark :</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Depending on the device model or the advanced parameters which you selected, the generation can take quite a long time. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Generally, for a Full-HD 1080p video: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>On a PC with 4 fast cores (Core i7 for example) : The process will take approximately 2 minutes for each minute of video to be rendered</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>On a PC with 2 cores (Core 2 duo for example) : The process will take approximately 5 minutes for each minute of video to be rendered</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This amounts to 2 to 5 hours for a one-hour video. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This duration will be much shorter for a video in VGA resolution. That is why we recommend you to carry out all tests and focusing trials at low resolution, and to render your videos in Full-HD only for the last step. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>ISO 639</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This dialog allows you to render the project as a video file.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This value is used by players to indicate in which language is the audio track. This value is on 3 characters and follows this recommendation: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Bitrate</source>
        <translation type="unfinished">Битрейт</translation>
    </message>
    <message>
        <source>This dialog has different appearances depending on whether you clicked the Device type, Lossless, Advanced or Soundtrack button. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Join thumbnail to rendered file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If this box is checked then a thumbnail file will be created with video. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Join XBMC .nfo file to rendered file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If this box is checked then a &lt;a href=&quot;0044.html&quot;&gt;XBMC .nfo&lt;/a&gt; file will be created with video. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Select the audio compression ratio from the displayed list</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Frequency</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>In export soundtrack mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Select the type of file to be used to render the soundtrack from the displayed list.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>At the dialog bottom</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Note: the project properties are saved into the video as TAG (METADATA) which are used by a lot of indexing programs or by </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Media Center Systems</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>For more information on joined files, see &lt;a href=&quot;003A.html&quot;&gt;File attachments&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Select the frequency to use for audio during the rendering.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> The properties button allows you to open the &lt;a href=&quot;0108.html&quot;&gt;Project properties dialog&lt;/a&gt;. It allows you to adjust the properties before generating the video. </source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0116</name>
    <message>
        <source>Ruler properties</source>
        <translation>Свойства линеек</translation>
    </message>
    <message>
        <source>Edge of the screen</source>
        <translation>Края экрана</translation>
    </message>
    <message>
        <source>Corresponds to the edge of the screen.</source>
        <translation>соответствует краям экрана.</translation>
    </message>
    <message>
        <source>TV margins</source>
        <translation>Поля ТВ</translation>
    </message>
    <message>
        <source>In ffDiaporama, the TV margins are of type &quot;Title safe area&quot; and are defined on 90% in width and 90% in height, for all the project geometries.</source>
        <translation>в ffDiaporama, поля телевизора имеют тип &quot;Безопасная зона&quot; и определяются на 90% в ширину и 90% в высоту, для всех геометрий проекта.</translation>
    </message>
    <message>
        <source>Centre of the screen</source>
        <translation>Центр экрана</translation>
    </message>
    <message>
        <source>Corresponds to the center of the screen.</source>
        <translation>соответствует центру экрана.</translation>
    </message>
    <message>
        <source>Unselected blocks</source>
        <translation>Невыделенные объекты</translation>
    </message>
    <message>
        <source>For all the objects which are not a part of the selection, create rulers for the top, bottom, left and right edges of blocks as well as for the centers of blocks.</source>
        <translation>для всех объектов, которые не являются частью выделения, создаются линейки по верхней, нижней, левой и правой границе блоков, а также для центров блоков.</translation>
    </message>
    <message>
        <source>Remarks:</source>
        <translation>Примечания:</translation>
    </message>
    <message>
        <source>Rulers can be activated or deactivated one by one.</source>
        <translation>Линейки могут быть включены или откоючены по отдельности.</translation>
    </message>
    <message>
        <source>When a ruler is activated, the mouse is attracted towards this ruler. This is valid as well for the movements of blocks as for the resizing (movements of the handles).</source>
        <translation>Когда линейка включена, мышь притягивается к ней. Это справедливо как для перемещения блоков, так и для изменения размера ( перемещение маркера).</translation>
    </message>
    <message>
        <source>TV margins rullers are not available for thumbnails editing.</source>
        <translation>Линейки Поля ТВ не доступны при редактировании обложки.</translation>
    </message>
    <message>
        <source>See also</source>
        <translation>Также смотрите</translation>
    </message>
    <message>
        <source>Description of TV margins and overscan</source>
        <translation>Описание телевизионных полей и оверскан (вылеты развертки)</translation>
    </message>
    <message>
        <source>This dialog allows you to enable or disable rulers.</source>
        <translation>Это диалоговое окно позволет включить или отключить линейки.</translation>
    </message>
</context>
<context>
    <name>0117</name>
    <message>
        <source>Edit thumbnail model</source>
        <translation type="unfinished">Изменение шаблона обложки</translation>
    </message>
</context>
<context>
    <name>0118</name>
    <message>
        <source>Set first shot duration for a set of slides</source>
        <translation type="unfinished">Выбор продолжительности первого кадра для группы слайдов</translation>
    </message>
    <message>
        <source> This dialog allows you to define the duration of the fist shot of a set of slides: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>First shot duration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Define the duration of the first shot.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>When you click the OK button, the new duration is applied to all the slides that were selected. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This new duration affects only the first shot of each slide</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This new duration is applied only to standard slides. The predefined title slides are not impacted.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0119</name>
    <message>
        <source>Slide properties</source>
        <translation type="unfinished">Параметры слайда</translation>
    </message>
    <message>
        <source>Remarks: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Toolbar</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Slide settings zone</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shots table</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Blocks table</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shot settings zone</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Interactive zone</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Block settings zone</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> The toolbar allows to launch the main slide editing commands. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Append a shot to the end of the shot table</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remove current selected shot</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Change the order of the shots by moving the current selected shot a step to the left</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Change the order of the shots by moving the current selected shot a step to the right</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Copy the current block to the ffDiaporama clipboard and remove it from the slide. The copy is made with the block parameters defined for the current selected shot.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Copy the current block to the ffDiaporama clipboard. The copy is made with the block parameters defined for the current selected shot.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Paste block from the ffDiaporama clipboard and insert it to the end of the block table</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remove the current block</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Change the order of blocks by moving the active block upward (towards the background)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Change the order of blocks by moving the active block downward (towards the foreground)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Make the active block visible or hidden. Note: Hidden video blocks are automatically paused</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Play the sound, during this shot, of the selected video block. (This button is active only for videos.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Open the &lt;a href=&quot;0110.html&quot;&gt;Image framing and correction dialog&lt;/a&gt;. This button is active only for objects containing images or videos.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Block properties locking</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Display information on active block. (Remark: This function is available only for &quot;image&quot; or &quot;video&quot; blocks.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Align to top</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Align to midle</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Align to bottom</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Align to left</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Align to center</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Align to right</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Distribute horizontally</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Distribute vertically</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Name of the slide</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Indicate here a name for the slide. It is this name which will appear on the thumbnail in the main window timeline.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Total slide duration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The total slide duration is displayed here. This duration takes into account possible adjustments for minimal duration of the shot as described above.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> The shots table shows the shots as thumbnails, in the order in which they will be played (from left to right). </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>For each shot, the thumbnail shows: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The shot duration (At the top of the thumbnail)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The temporal position of the shot at the end of its duration (At the bottom right of the thumbnail)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>An image representing the shot at the end of its duration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remark:</source>
        <translation type="unfinished">Примечание:</translation>
    </message>
    <message>
        <source>The order of the shots can be changed by using Drag &amp; Drop.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shot duration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> Indicate here the duration of the currently selected shot. For the last shot of the slide, there is a minimum duration.The following rules are used to calculate the minimum duration of the shot: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The total slide duration cannot be lower than the sum of the durations of the entering transition of the slide plus that of the transition to the following slide</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The total slide duration must permit complete play of the longest video sequence (time of breaks is taken into account)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The total slide duration cannot be lower than 1:30 second</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> The interactive zone allows you to move and interactively re-size blocks within the selected shot. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Selected blocks are shown with a frame. (It allows to differentiate them from other graphic elements of the slide).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Control handles (small square) are shown on sides and on corners of the selection frame.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Rulers are displayed to facilitate the work with the mouse. See &lt;a href=&quot;0116.html&quot;&gt;Rulers settings&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Using the mouse:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To select a block: click the block</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To select multiple blocks: Select a first block by clicking on it and then, by clicking on the other blocks to be selected with pressing either the SHIFT or the CTRL key.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To select a block under another block : Click the block to be selected by pushing both SHIFT and CTRL keys.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To unselect a block in a multiple selection: Click the block with pressing either the SHIFT or the CTRL key.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To modify the size of the blocks of the selection: Click one of the handles of the selection and, while keeping the button of the mouse pressed, move the mouse.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To move the blocks of the selection: Click one of the blocks of the selection and, while keeping the button of the mouse pressed, move the mouse.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Using the keyboard:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To move the blocks of the selection, use the arrow keys.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To change the size of the blocks of the selection, use SHIFT+arrows to move the upper left corner handle or CTRL+arrows to move the lower right corner handle.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remarks:</source>
        <translation type="unfinished">Примечания:</translation>
    </message>
    <message>
        <source>A double click on a block opens the block editor associated with the block type (for a text block this will be the text editor, for an image block the reframing dialog, etc.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A right click on a block when the selection includes only a single block opens a contextual menu proposing the editing commands for a block.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A right click on a block when the selection includes multiple blocks opens a contextual menu proposing the editing commands for multiple blocks.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The order of the blocks is important! The first block (at the top of the list) is covered by the second block which is even covered by the following block and so on.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The order of the blocks is defined shot by shot.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The first column contains or: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> If the block is a title.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A thumbnail if the block is an image or a video.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> If the image or the video have a transformation filter.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The second column contains icons giving the state of the blocks for the current shot: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> If the video has the sound during this shot. This information appears only for the video blocks.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The third column indicates: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If the block is a title: the text of the block</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If the block is an image: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The filename.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The width x the height of the image in pixel (the format of the image - the geometry of the image).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A summary of EXIF information on the image.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If the block is a video: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The duration of the video.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Double clicking on a block opens the block editor associated with the block type (for a text block this will be the text editor, for an image block the reframing dialog, etc.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Right clicking on a block when the selection includes only a single block opens a contextual menu proposing the editing commands for a block.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Right clicking on a block when the selection includes multiple blocks opens a contextual menu proposing the editing commands for multiple blocks.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Blocks with locked properties are shown on a grey bottom.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Selected blocks are shown on a red bottom if the focus is on the blocks table or on a blue bottom if the focus is on another element.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To change the blocks order, use Drag &amp; Drop or &lt;B&gt;Up&lt;/B&gt; and &lt;B&gt;Down&lt;/B&gt; functions of the contextual menu.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>It is possible to add files directly in the blocks table by using Drag and Drop with the file explorer.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>All blocks, whether title, image or video, possess a text element.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>During the insertion of a new block (Add title, Add file or Paste), the block is inserted: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>In visible mode for the current shot and for all the following shots</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>In hidden mode for the shots located before the current shot</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Always in the last position in the list of blocks (in the foreground)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Coordinates</source>
        <translation type="unfinished">Координаты</translation>
    </message>
    <message>
        <source>&lt;B&gt;For image or video blocks: &lt;/B&gt;Allows you to select a reframing style for the image. It has the same effect as selecting a style in the dialog: &lt;a href=&quot;0110.html&quot;&gt;Correct, reframe or cut image or video&lt;/a&gt;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&lt;B&gt;For text blocks: &lt;/B&gt;Allows you to select the geometrical constraints to be applied to the block. Three type of constraint are available:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>No constraint is applied to the selection rectangle.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Lock selection rectangle to the actual geometry. It has no meaning until you have defined a shape in &quot;no constraint&quot; mode.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Lock selection rectangle to the same geometry as the project, such that, if you place the image in full screen, the reframed part will correspond to the whole screen.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Allows you to select a pre-defined block size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Represents the horizontal offset of the block from the left. This position is defined as a percentage (%) of the total width of the image or in pixels (for a 1080p screen) according to the unit defined in the configuration dialog.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Represents the vertical offset of the block from the top. This position is defined as a percentage (%) of the total height of the image or in pixels (for a 1080p screen) according to the unit defined in the configuration dialog.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Represents the width of the block. This value is defined as a percentage (%) of the total width of the image or in pixels (for a 1080p screen) according to the unit defined in the configuration dialog box.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Represents the height of the block. This value is defined as a percentage (%) of the total height of the image or in pixels (for a 1080p screen) according to the unit defined in the configuration dialog.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Rotation around the Z axis in degrees.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Rotation around the Y axis in degrees.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Rotation around the X axis in degrees.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Block shape</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This settings group deals with style management. For more information on style management, see : Using style management</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shape</source>
        <translation type="unfinished">Форма</translation>
    </message>
    <message>
        <source>Allows you to select a shape for the block. The selected shape will be used as:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>a fusion mask for the images and the videos.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A cutting zone for texts</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Opacity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the opacity of the block. This value also applies to the text of the block.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Border</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Thickness of the border</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the thickness of the border (This value is defined as 1/1080 of the height of the screen. A value of 10 corresponds to 10 points on a 1920x1080 FullHD screen, but only to 6 points on a 1280x720 screen).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Border color</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the color of the line to be used to draw the border</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Line type of border</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the type of line (solid, dotted line, etc.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shadow</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shadow position</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines in which corner the shadow will be positioned (Note: the position of the shadow is independent of rotation of the block: the right lower corner is always the right lower corner, whatever rotation is applied).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Distance of the shadow</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the distance between the block and its shadow</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Color</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Animation option</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Speed wave</source>
        <translation type="unfinished">Скорость</translation>
    </message>
    <message>
        <source>Selects a speed curve for block animations (movement, resizing and rotating of blocks, animation of text and blocks) </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remark: To change the velocity curve for image animation (graphic effect and framing), edit the image or video and select a velocity in the Image framing and correction dialog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Text annimation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Zoom</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines a level of zoom for the text</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the horizontal position of the text inside the block</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the vertical position of the text inside the block</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Define values for zoom and scrolling for the 1st shot</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Define different values for zoom and scrolling for the 2nd</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Block animation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> The block animations define which animations will run during the current shot. You can define different animations for every shot! </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Multiple rotations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This animation allows to turn a block several times during the shot. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remark: the rotation direction is defined by the sign, so +5 will give 5 rotations in one direction while -5 will give 5 rotations in the other direction </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the number of rotation to be made according to the Z axis.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the number of rotation to be made according to the Y axis.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Defines the number of rotation to be made according to the X axis.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Dissolve</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This animation dissolves between the block and a transparent zone according to the following settings.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Appear</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The block will appear during the shot</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Disappear</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The block will disappear during the shot</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Blink</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The block will blink during the shot (appear then disappear). 4 speeds are proposed (from slow to very fast).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> When the block properties are locked, the modifications made to the block in the preceding shot are echoed in the current shot. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>It defines as well the coordinates, forms and animations properties as the properties defined in the editing dialog of text, image or video. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The properties locking is available only from the 2nd shot.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>See also</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Description of EXIF - Exchangeable image file format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This settings group defines the coordinates of the block for the current shot, so, the modifications made to the following properties will apply only to current shot. To understand the differences between the properties which apply to all shots and those which apply only to the current shot, consult: &lt;a href=&quot;0010.html&quot;&gt;How ffDiaporama works&lt;/a&gt;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> This settings group defines the shape of the block for all shots, so, modification of the following properties will apply to all shots in the slide. To understand the differences between the properties which apply to all shots and those which apply only to the current shot, consult: &lt;a href=&quot;0010.html&quot;&gt;How ffDiaporama works&lt;/a&gt;. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>On the top: The &lt;a href=&quot;#Toolbar&quot;&gt;Toolbar&lt;/a&gt; allows to launch the main slide editing commands, and the &lt;a href=&quot;#ShotSettings&quot;&gt;Shot settings&lt;/a&gt; zone.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>On the left: The &lt;a href=&quot;#SlideSettings&quot;&gt;Slide settings zone&lt;/a&gt;, the &lt;a href=&quot;#ShotsTable&quot;&gt;Shots table&lt;/a&gt; and the &lt;a href=&quot;#InteractiveZone&quot;&gt;Interactive&lt;/a&gt; zones.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>On the right: The &lt;a href=&quot;#BlockTable&quot;&gt;Blocks table&lt;/a&gt; and the &lt;a href=&quot;#BlockSettings&quot;&gt;Block settings&lt;/a&gt; zones.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Open the &lt;a href=&quot;0120.html&quot;&gt;Edit text block dialog&lt;/a&gt;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Lock or unlock properties of active block. (See: &lt;a href=&quot;#LockProperties&quot;&gt;Block properties locking&lt;/a&gt;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> If the block is visible during this shot.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> If the properties of the block are locked during this shot. (This information does not appear for the 1st plan.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> If the properties of the block are unlocked during this shot. (This information does not appear for the 1st plan.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> If the block is hidden during this shot.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Sets the color used to draw the shadow</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If you specify a duration lower than the minimum calculated duration for the shot, it is this minimum duration that will be used in place of the value which you specified.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Blocks are shown on 3 columns:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The blocks table lists, from top to bottom, the present blocks in the slide.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To make animations of text, you have to create at least 2 shots then:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This dialog allows you to define settings for the slides. It includes numerous zones:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Open the &lt;a href=&quot;0105.html&quot;&gt;Chapter properties dialog&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Add a new title block - Open a sub menu with the following options: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Add a simple title block</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Add a title clip-art block</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Once you have selected one of the options, the &lt;a href=&quot;0120.html&quot;&gt;Edit Text dialog&lt;/a&gt; opens.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Add one or several new blocks of images or video. A &lt;a href=&quot;0109.html&quot;&gt;file selection dialog&lt;/a&gt; appears allowing you to select files. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Note: in case of multiple selection, files are added in the alphabetical order of the filenames or in the digital order of shots (according to the option selected in the &lt;a href=&quot;0101.html&quot;&gt;Application options dialog&lt;/a&gt;).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Edit blocks - Open a sub menu with the following options: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Arrange blocks - Open a sub menu with the following options: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Save as model - Allows you to save current slide as a new Predefined custom title. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>For more information on predefined title slide, see: &lt;a href=&quot;0012.html&quot;&gt;Predefined title slide&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> At the bottom of the box, the &quot;Rullers&quot; button allows you to open the &lt;a href=&quot;0116.html&quot;&gt;Ruler properties dialog&lt;/a&gt; </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source> It is also possible to open contextual menus by: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Right clicking on zones.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Right click on shots table.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Right click on blocks table or on interactive zone when no block was selected.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Right click on one of the blocks of a multiple selection, on blocks table or on interactive zone.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Each of these contextual menus propose commands which depend on what you have clicked.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0120</name>
    <message>
        <source>Edit text</source>
        <translation>Редактирование текста</translation>
    </message>
    <message>
        <source>Properties for all shots</source>
        <translation>Параметры для всех кадров</translation>
    </message>
    <message>
        <source> The modifications made to the following properties will apply to all shots of the slide. </source>
        <translation>Изменения, сделанные следующими настройками, будут применяться ко всем кадрам слайда.</translation>
    </message>
    <message>
        <source>Text tab</source>
        <translation>Вкладка Текст</translation>
    </message>
    <message>
        <source>First line at the top of the dialog</source>
        <translation>Первая строка верхней части диалогового окна</translation>
    </message>
    <message>
        <source>Allows you to select the font to be used, its size and its colour. This setting applies to the current selection.</source>
        <translation>Позволяет выбрать шрифт, его размер и цвет. Этот параметр применяется к текущему выделению.</translation>
    </message>
    <message>
        <source>Allows to establish (or to remove) a bullet list. This setting applies to the paragraphs of the current selection.</source>
        <translation>Позволяет установить (или удалить) маркированный список. Этот параметр применяется к текущему выделению.</translation>
    </message>
    <message>
        <source>Allows to establish (or to remove) a numbered list. This setting applies to the paragraphs of the current selection.</source>
        <translation>Позволяет установить (или удалить) нумерованный список. Этот параметр применяется к текущему выделению.</translation>
    </message>
    <message>
        <source>Horizontal alignment: left. This setting applies to the paragraph in which the cursor is.</source>
        <translation>Горизонтальное выравнивание: по левому краю. Этот параметр применяется к абзацу, в котором находится курсор.</translation>
    </message>
    <message>
        <source>Horizontal alignment: centered. This setting applies to the paragraph in which the cursor is.</source>
        <translation>Горизонтальное выравнивание: по центру. Этот параметр применяется к абзацу, в котором находится курсор.</translation>
    </message>
    <message>
        <source>Horizontal alignment: justified. This setting applies to the paragraph in which the cursor is.</source>
        <translation>Горизонтальное выравнивание: по ширине. Этот параметр применяется к абзацу, в котором находится курсор.</translation>
    </message>
    <message>
        <source>Horizontal alignment: right. This setting applies to the paragraph in which the cursor is.</source>
        <translation>Горизонтальное выравнивание: по правому краю. Этот параметр применяется к абзацу, в котором находится курсор.</translation>
    </message>
    <message>
        <source>Second line at the top of the dialog</source>
        <translation>Вторая строка верхней части диалогового окна</translation>
    </message>
    <message>
        <source>Allows you to add outline or shadow to the whole text: </source>
        <translation>Позволяет добавить контур или тень ко всему тексту: </translation>
    </message>
    <message>
        <source>The first list allows you to select the effect: </source>
        <translation>Первый список позволяет выбрать эффект: </translation>
    </message>
    <message>
        <source>No effect</source>
        <translation>Без эффекта</translation>
    </message>
    <message>
        <source>Text is shown without any shadow or outline</source>
        <translation>Текст отображается без тени или контура</translation>
    </message>
    <message>
        <source>Outline</source>
        <translation>Контур</translation>
    </message>
    <message>
        <source>Surrounds the characters with a line in the current shadow color</source>
        <translation>Обводит символы линией цвета текущей тени</translation>
    </message>
    <message>
        <source>Shadow ...</source>
        <translation>Тень ...</translation>
    </message>
    <message>
        <source>Add a shadow in the current shadow color in one of the 4 corners.</source>
        <translation>Добавление тени текущего цвета в один из 4 углов.</translation>
    </message>
    <message>
        <source>The second list allows you to select the color of the shadow or the outline.</source>
        <translation>Второй список позволяет выбрать цвет тени или контура.</translation>
    </message>
    <message>
        <source>Sets bold characters. This setting applies to the current selection.</source>
        <translation>Жирный. Этот параметр применяется к текущему выделению.</translation>
    </message>
    <message>
        <source>Sets characters to italics. This setting applies to the current selection.</source>
        <translation>Курсив. Этот параметр применяется к текущему выделению.</translation>
    </message>
    <message>
        <source>Underlines the characters. This setting applies to the current selection.</source>
        <translation>Подчеркнутый. Этот параметр применяется к текущему выделению.</translation>
    </message>
    <message>
        <source>Puts the characters to super. This setting applies to the current selection.</source>
        <translation>Надстрочный. Этот параметр применяется к текущему выделению.</translation>
    </message>
    <message>
        <source>Puts the characters to sub. This setting applies to the current selection.</source>
        <translation>Подстрочный. Этот параметр применяется к текущему выделению.</translation>
    </message>
    <message>
        <source>Move the paragraph of a notch towards the right (indent). This setting applies to the paragraph in which is the cursor.</source>
        <translation>Увеличить отступ. Этот параметр применяется к абзацу, в котором находится курсор.</translation>
    </message>
    <message>
        <source>Move the paragraph of a notch towards the left (unindent). This setting applies to the paragraph in which is the cursor.</source>
        <translation>Уменьшить отступ. Этот параметр применяется к абзацу, в котором находится курсор.</translation>
    </message>
    <message>
        <source>Vertical alignment: top. This setting applies to the block (to the whole text).</source>
        <translation>Вертикальное выравнивание: по верху. Этот параметр применяется к блоку (для всего текста).</translation>
    </message>
    <message>
        <source>Vertical alignment: middle. This setting applies to the block (to the whole text).</source>
        <translation>Вертикальное выравнивание: по середине. Этот параметр применяется к блоку (для всего текста).</translation>
    </message>
    <message>
        <source>Vertical alignment: bottom. This setting applies to the block (to the whole text).</source>
        <translation>Вертикальное выравнивание: по низу. Этот параметр применяется к блоку (для всего текста).</translation>
    </message>
    <message>
        <source>Background text tab</source>
        <translation>Вкладка Фон</translation>
    </message>
    <message>
        <source>&lt;U&gt;&lt;B&gt;&lt;I&gt;Note: This tab is available only for the title blocks (It&apos;s not available when the text applies to an image block).&lt;/I&gt;&lt;/B&gt;&lt;/U&gt;</source>
        <translation>&lt;U&gt;&lt;B&gt;&lt;I&gt;Примечание: эта вкладка доступна только для заголовков (она не доступна, когда текст накладывается на блок изображения).&lt;/I&gt;&lt;/B&gt;&lt;/U&gt;</translation>
    </message>
    <message>
        <source>Allows you to define a background brush for the shape. The following options are available: </source>
        <translation>Позволяет определить стиль фона для формы. Доступны следующие параметры:</translation>
    </message>
    <message>
        <source>No brush</source>
        <translation>Без кисти</translation>
    </message>
    <message>
        <source>The shape will have no background and will thus be totally transparent.</source>
        <translation>Форма не будет иметь никакого фона и, таким образом, будет полностью прозрачной.</translation>
    </message>
    <message>
        <source>Solid brush</source>
        <translation>Твердая кисть</translation>
    </message>
    <message>
        <source>Allows you to select a plain color.</source>
        <translation>Позволяет выбрать сплошной цвет.</translation>
    </message>
    <message>
        <source>Pattern brush</source>
        <translation>Кисть-узор</translation>
    </message>
    <message>
        <source>Allows you to select a hatched brush.</source>
        <translation>Позволяет выбрать штриховку.</translation>
    </message>
    <message>
        <source>Gradient 2 colors</source>
        <translation>Градиент из 2 цветов</translation>
    </message>
    <message>
        <source>Allows you to select a gradient of 2 colors.</source>
        <translation>Позволяет выбрать градиент, состоящий из 2 цветов.</translation>
    </message>
    <message>
        <source>Gradient 3 colors</source>
        <translation>Градиент из 3 цветов</translation>
    </message>
    <message>
        <source>Allows you to select a gradient of 3 colors.</source>
        <translation>Позволяет выбрать градиент, состоящий из 3 цветов.</translation>
    </message>
    <message>
        <source>Image from library</source>
        <translation>Изображение из библиотеки</translation>
    </message>
    <message>
        <source>Allows you to select an image from the library.</source>
        <translation>Позволяет выбрать изображение из библиотеки.</translation>
    </message>
    <message>
        <source>If background brush is Solid, Pattern or Gradient brush, then this line appears and allows you to select a color. (Depending on the brush you select, there will be one, two or three selectable colors.)</source>
        <translation>Если кисть фона - твердая, узор или градиент, то появляется эта настройка и позволяет выбрать цвет. (В зависимости от выбора кисти, там будет один, два или три цвета.)</translation>
    </message>
    <message>
        <source>If background brush is Pattern brush, then this line appears and allows you to select a pattern.</source>
        <translation>Если кисть фона - узор, то  появляется эта настройка и позволяет выбрать шаблон.</translation>
    </message>
    <message>
        <source>If background brush is Gradient, then this line appears and allows you to select the gradient orientation. </source>
        <translation>Если кисть фона -  градиент, то появляется эта настройка и позволяет выбрать направление градиента.</translation>
    </message>
    <message>
        <source>If the Gradient has 3 colors, additional controls allow to define the position (in percentage) of the 3rd gradient color.</source>
        <translation>Если градиент имеет 3 цвета, дополнительные элементы управления позволяют определить положение (в процентах) 3-го цвета градиента.</translation>
    </message>
    <message>
        <source>If background brush is Image from library, then this line appears and allows you to select an image.</source>
        <translation>Если кисть фона -  изображение из библиотеки, то появляется эта настройка и позволяет выбрать изображение.</translation>
    </message>
    <message>
        <source>Margins tab</source>
        <translation>Вкладка Поля</translation>
    </message>
    <message>
        <source>Default margins</source>
        <translation>Поля по умолчанию</translation>
    </message>
    <message>
        <source>To use predefined margins for the shape of the block. In general, these margins ensure that the text is contained in the form.</source>
        <translation>Позволяет использовать предопределенные поля для формы блока. Эти поля гарантируют, что текст поместится в форме.</translation>
    </message>
    <message>
        <source>Full shape</source>
        <translation>Полная форма</translation>
    </message>
    <message>
        <source>To use margins corresponding to the entire form, in fact, the selection rectangle shape regardless of the design.</source>
        <translation>Позволяет использовать поля, соответствующие полной форме, то есть форме прямоугольника выделения, независимо от дизайна.</translation>
    </message>
    <message>
        <source>Custom</source>
        <translation>Пользовательские</translation>
    </message>
    <message>
        <source>To use custom margins. The following settings allow you to adjust the margins: </source>
        <translation>Позволяет использовать настраиваемые поля. Для настройки доступны следующие параметры: </translation>
    </message>
    <message>
        <source>Represents the horizontal position of the margins relative to the top-left corner of the block. (This value is expressed in % of the width of the block.)</source>
        <translation>Горизонтальная позиция поля от верхнего левого угла блока. (Это значение выражается в % от ширины блока)</translation>
    </message>
    <message>
        <source>Represents the vertical position of the margins relative to the top-left corner of the block. (This value is expressed in % of the height of the block.)</source>
        <translation>Вертикальная  позиция поля от верхнего левого угла блока.  (Это значение выражается в % от высоты блока)</translation>
    </message>
    <message>
        <source>Represents the width of the margins. (This value is expressed in % of the width of the block.)</source>
        <translation>Ширина поля. (Это значение выражается в % от ширины блока)</translation>
    </message>
    <message>
        <source>Represents the height of the margins. (This value is expressed in % of the height of the block.)</source>
        <translation>Высота поля. (Это значение выражается в % от высоты блока)</translation>
    </message>
    <message>
        <source>This dialog allows you to modify the text within a block.</source>
        <translation>Это диалоговое окно позволяет изменить текст внутри блока.</translation>
    </message>
    <message>
        <source>To understand indeed the differences between the properties which apply to all shots and those who apply only to the current shot, consult: &lt;a href=&quot;0010.html&quot;&gt;How ffDiaporama works&lt;/a&gt;. </source>
        <translation>Чтобы действительно понять различия между свойствами, которые применяются ко всем кадрам, и свойствами, которые применяются только к текущему кадру, посмотрите: &lt;a href=&quot;0010.html&quot;&gt; Принципы работы ffDiaporama&lt;/a&gt;.</translation>
    </message>
    <message>
        <source>This settings group is the object of style management. for more information on style management, see : &lt;a href=&quot;0016.html&quot;&gt;Using style management&lt;/a&gt;</source>
        <translation>Эта группа параметров является объектом управления стилями. Дополнительные сведения об управлении стиялми, см.: &lt;a href=&quot;0016.html&quot;&gt; Использование управления стилями&lt;/a&gt;</translation>
    </message>
    <message>
        <source>Allows you to add a dynamic variable to the text. For more information on dynamic variables see: &lt;a href=&quot;0013.html&quot;&gt;Dynamic variables&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0121</name>
    <message>
        <source>Select transition duration for a set of slides</source>
        <translation type="unfinished">Выбор продолжительности перехода для группы слайдов</translation>
    </message>
    <message>
        <source> This dialog allows you to define the transition duration for a set of slides: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Duration</source>
        <translation type="unfinished">Продолжительность</translation>
    </message>
    <message>
        <source>Define the animation duration.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>When you click the OK button, the new transition duration is applied to all the slides that were selected. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>This function can not be used to delete or change the slide&apos;s transition. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>If it&apos;s what you want, then call the context transitions menu by right clicking on one of the slides of the selection from the &lt;a href=&quot;0023.html&quot;&gt;timeline&lt;/a&gt; and then select &quot;Modify the transition for all the slides of the selection&quot; option.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>0122</name>
    <message>
        <source>Transition properties</source>
        <translation>Параметры перехода</translation>
    </message>
    <message>
        <source> This dialog allows you to define the entering transition for a slide. </source>
        <translation>Это диалоговое окно позволяет определить входящий переход слайда. </translation>
    </message>
    <message>
        <source>It shows a preview of the available transitions by transition family. </source>
        <translation>Оно показывает предварительный просмотр доступных переходов по типам.</translation>
    </message>
    <message>
        <source>Remark : In case this dialog is opened for a multiple slide selection, it&apos;s the first slide of the selection that is used for the preview. </source>
        <translation>Примечание: в случае, если это диалоговое окно открыто при выделении нескольких слайдов, то для предварительного просмотра используется первый слайд.</translation>
    </message>
    <message>
        <source>Transition type</source>
        <translation>Тип перехода</translation>
    </message>
    <message>
        <source>No transition</source>
        <translation>Без перехода</translation>
    </message>
    <message>
        <source>Note that this also deactivates:</source>
        <translation>Обратите внимание, что это также отключает:</translation>
    </message>
    <message>
        <source>Sound fades (The playlist of the new slide immediately replaces that of the old one)</source>
        <translation>Затухание звука (плейлист нового слайда немедленно заменяет старый)</translation>
    </message>
    <message>
        <source>Background transitions</source>
        <translation>Фоновые переходы</translation>
    </message>
    <message>
        <source>Basic</source>
        <translation>Базовые</translation>
    </message>
    <message>
        <source>Scripted transition</source>
        <translation>Запрограммированные переходы</translation>
    </message>
    <message>
        <source>LUMA transition</source>
        <translation>LUMA переходы</translation>
    </message>
    <message>
        <source>The animation is based on a luminance mask : the darker the points in the mask, the faster the corresponding points in the new slide replace those in the old slide.</source>
        <translation>Анимация основана на маске яркости: темные точки в маске быстрее заменяются соответствующими точками нового слайда.</translation>
    </message>
    <message>
        <source>Speed wave</source>
        <translation>Скорость</translation>
    </message>
    <message>
        <source>Allows you to select a speed curve to apply to the transition</source>
        <translation>Позволяет выбрать скорость перехода</translation>
    </message>
    <message>
        <source>Duration</source>
        <translation>Продолжительность</translation>
    </message>
    <message>
        <source>Allows you to define the duration of the animation. </source>
        <translation>Позволяет установить продолжительность анимации.</translation>
    </message>
    <message>
        <source>The possible choices are of 0,5 seconds (very fast) to 10 seconds (very slow)</source>
        <translation>Возможны варианты от 0,5 секунд (очень быстро) до 10 секунд (очень медленно)</translation>
    </message>
    <message>
        <source>See also</source>
        <translation>Смотрите также</translation>
    </message>
    <message>
        <source>Description of LUMA</source>
        <translation>Описание LUMA</translation>
    </message>
    <message>
        <source>Allows you to select a transition family: </source>
        <translation>Позволяет выбрать тип перехода:</translation>
    </message>
    <message>
        <source>No transition: The new slide immediately replaces the old one. </source>
        <translation>Нет перехода: новый слайд немедленно заменяет старый.</translation>
    </message>
    <message>
        <source>Dissolve effect: The slide appears gradually, with gradually increasing opacity.</source>
        <translation>Эффект растворения: слайд появляется постепенно, с постепенно возрастающей непрозрачностью.</translation>
    </message>
    <message>
        <source>Zoom: The new slide appears by getting bigger while the old slide disappears with a dissolve effect. </source>
        <translation>Масштабирование: новый слайд появляется все больше и больше, в то время как старый слайд исчезает с эффектом растворения.</translation>
    </message>
    <message>
        <source>Slide: The new slide glides onto the old slide which disappears with a dissolve effect. </source>
        <translation>Скольжение: новый слайд скользит на старый слайд, который исчезает с эффектом растворения.</translation>
    </message>
    <message>
        <source>Push: The new slide takes the place of the old slide by pushing it aside.</source>
        <translation>Сталкивание: новый слайд занимает место старого,сталкивая его в сторону.</translation>
    </message>
</context>
<context>
    <name>QHP</name>
    <message>
        <source>Welcome to the ffDiaporama WIKI</source>
        <translation>Добро пожаловать в ffDiaporama WIKI</translation>
    </message>
    <message>
        <source>How ffDiaporama works</source>
        <translation>Принципы работы ffDiaporama</translation>
    </message>
    <message>
        <source>Functional principles</source>
        <translation>Принципы функционирования</translation>
    </message>
    <message>
        <source>The Visible property of blocks and the management of the breaks</source>
        <translation>Свойство &quot;видимости&quot; блоков и управление переходами между ними</translation>
    </message>
    <message>
        <source>All shots regulations versus current shot regulations</source>
        <translation>Настройки всех кадров и настройки текущего кадра</translation>
    </message>
    <message>
        <source>Style sheets</source>
        <translation>Таблицы стилей</translation>
    </message>
    <message>
        <source>Interface description</source>
        <translation>Описание интерфейса</translation>
    </message>
    <message>
        <source>The main window</source>
        <translation>Главное окно</translation>
    </message>
    <message>
        <source>The toolbar</source>
        <translation>Панель инструментов</translation>
    </message>
    <message>
        <source>The timeline</source>
        <translation>Линия времени (таймлайн)</translation>
    </message>
    <message>
        <source>The multimedia file browser</source>
        <translation>Мультимедиа Проводник</translation>
    </message>
    <message>
        <source>Rendering videos</source>
        <translation>Создание видео</translation>
    </message>
    <message>
        <source>Smartphones and tablets</source>
        <translation>Смартфоны и планшеты</translation>
    </message>
    <message>
        <source>Portable devices</source>
        <translation>Портативные устройства</translation>
    </message>
    <message>
        <source>Multimedia system (Home cinema)</source>
        <translation>Мультимедиа система (домашний кинотеатр)</translation>
    </message>
    <message>
        <source>For the WEB</source>
        <translation>Интернет</translation>
    </message>
    <message>
        <source>Video codec</source>
        <translation>Видео кодек</translation>
    </message>
    <message>
        <source>Audio codec</source>
        <translation>Аудио кодек</translation>
    </message>
    <message>
        <source>Containers</source>
        <translation>Контейнеры</translation>
    </message>
    <message>
        <source>Video standards and resolutions</source>
        <translation>Видео стандарты и разрешения</translation>
    </message>
    <message>
        <source>File attachments (Thumbnails, nfo, etc...)</source>
        <translation>Присоединяемые файлы (обложка, nfo, и др.)</translation>
    </message>
    <message>
        <source>Technical information on ffDiaporama</source>
        <translation>Техническая информация о ffDiaporama</translation>
    </message>
    <message>
        <source>ffDiaporama command line arguments</source>
        <translation>Параметры командной строки ffDiaporama</translation>
    </message>
    <message>
        <source>ffDiaporama project files</source>
        <translation>Файлы проектов ffDiaporama</translation>
    </message>
    <message>
        <source>The configuration files for ffDiaporama</source>
        <translation>Файлы конфигурации ffDiaporama</translation>
    </message>
    <message>
        <source>XBMC nfo files</source>
        <translation>XBMC nfo файлы</translation>
    </message>
    <message>
        <source>Dialog boxes</source>
        <translation>Диалоговые окна</translation>
    </message>
    <message>
        <source>Application options</source>
        <translation>Настройки программы</translation>
    </message>
    <message>
        <source>Manage devices</source>
        <translation>Управление устройствами</translation>
    </message>
    <message>
        <source>Add or modify a predefined title slide</source>
        <translation>Добавление или изменение слайда из шаблона</translation>
    </message>
    <message>
        <source>Background properties</source>
        <translation>Свойства фона</translation>
    </message>
    <message>
        <source>Chapter properties</source>
        <translation>Свойства главы</translation>
    </message>
    <message>
        <source>Check configuration</source>
        <translation>Проверка конфигурации</translation>
    </message>
    <message>
        <source>Export project</source>
        <translation>Экспорт проекта</translation>
    </message>
    <message>
        <source>Project properties</source>
        <translation>Свойства проекта</translation>
    </message>
    <message>
        <source>Select files</source>
        <translation>Выбор файлов</translation>
    </message>
    <message>
        <source>Correct, reframe or cut image or video</source>
        <translation>Коррекция, кадрирование или обрезка изображения или видео</translation>
    </message>
    <message>
        <source>File information</source>
        <translation>Информация о файле</translation>
    </message>
    <message>
        <source>Manage favorite</source>
        <translation>Управление избранным</translation>
    </message>
    <message>
        <source>Manage style</source>
        <translation>Управление стилями</translation>
    </message>
    <message>
        <source>Music properties</source>
        <translation>Параметры музыки</translation>
    </message>
    <message>
        <source>Render video</source>
        <translation>Создание видео</translation>
    </message>
    <message>
        <source>Ruler properties</source>
        <translation>Свойства линеек</translation>
    </message>
    <message>
        <source>Edit thumbnail model</source>
        <translation>Изменение шаблона обложки</translation>
    </message>
    <message>
        <source>Set first shot duration for a set of slides</source>
        <translation>Выбор продолжительности первого кадра для группы слайдов</translation>
    </message>
    <message>
        <source>Slide properties</source>
        <translation>Параметры слайда</translation>
    </message>
    <message>
        <source>Edit text</source>
        <translation>Редактирование текста</translation>
    </message>
    <message>
        <source>Select transition duration for a set of slides</source>
        <translation>Выбор продолжительности перехода для группы слайдов</translation>
    </message>
    <message>
        <source>Transition properties</source>
        <translation>Параметры перехода</translation>
    </message>
    <message>
        <source>Dynamic variables</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>main</name>
    <message>
        <source>Welcome to the ffDiaporama WIKI</source>
        <translation>Добро пожаловать в ffDiaporama WIKI</translation>
    </message>
    <message>
        <source>How ffDiaporama works</source>
        <translation>Принципы работы ffDiaporama</translation>
    </message>
    <message>
        <source>Interface description</source>
        <translation>Описание интерфейса</translation>
    </message>
    <message>
        <source>Rendering videos</source>
        <translation>Создание видеофайлов</translation>
    </message>
    <message>
        <source>Technical information on ffDiaporama</source>
        <translation>Техническая информация о ffDiaporama</translation>
    </message>
    <message>
        <source> This documentation is divided into the followed themes: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>ffDiaporama Web site</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Finally, you&apos;ll also find help on the </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>ffDiaporama forum</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
