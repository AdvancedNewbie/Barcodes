const char ROOT_Page_Top[] PROGMEM = R"=====(
<!doctype html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.5, maximum-scale=1.5, user-scalable=no" />
    <style>
      body{
          font-family: "Libre Franklin", "Helvetica Neue", "helvetica", "arial", "sans-serif";
          font-size: 1.25rem;
      }

      input[type=submit] {
        box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2), 0 6px 20px 0 rgba(0,0,0,0.19);
        width: auto;
        min-width: 187px;
      }

      p{
          font-size: 0.5rem;
      }
    </style>
    <title>Test Title</title>
  </head>
  <body>
    <!-- <h5>Testing123...</h5> -->
      <div style="width: 100%">
        <form action="/submit" method="get">
            
          <label for="top">Top Text:</label><br>
          <input type="text" id="top" name="top"><br>
          
          <label for="code">11 Digit Code:</label><br>
          <p>(leave off last digit for checksum)</p>
          <input type="text" id="code" name="code"><br>
          
          <label for="bottom">Bottom Text:</label><br>
          <input type="text" id="bottom" name="bottom"><br>
          
          <!-- Save: <input type="checkbox" id="save" name="save" value="0"> -->

          </br>
          </br>
          </br>
          <input type="submit" value="Submit">
        </form>
      </div>
    
)=====";


const char ROOT_Page_Bottom[] PROGMEM = R"=====(
  </body>
</html>
)=====";
