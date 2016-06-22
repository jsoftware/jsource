coclass'jdemo12'
coinsert'jhs'

NB. glmatrix has 2 versions and they are incompatible
NB. glmatrix version 2 is used in this demo

NB. HEXTRA put in html after CSS and before JS (because some things need to come before HBS)
HEXTRA=: 0 : 0
<script type="x-shader/x-vertex" id="vshader">
 attribute vec3 coords;
 uniform mat4 modelview;
 uniform mat4 projection;
 uniform bool lit;
 uniform vec3 normal;
 uniform mat3 normalMatrix;
 uniform vec4 color;
 varying vec4 vColor;
 void main() {
  vec4 coords = vec4(coords,1.0);
  vec4 transformedVertex = modelview * coords;
  gl_Position = projection * transformedVertex;
  if (lit) {
   vec3 unitNormal = normalize(normalMatrix*normal);
   float multiplier = abs(unitNormal.z);
   vColor = vec4( multiplier*color.r, multiplier*color.g, multiplier*color.b, color.a );
  }
  else {
   vColor = color;
  }
 }
</script>
<script type="x-shader/x-fragment" id="fshader">
 precision mediump float;
 varying vec4 vColor;
 void main() {
  gl_FragColor = vColor;
 }
</script>
<script type="text/javascript" src="~addons/ide/jhs/js/webgl/glmatrix-min.js"></script>
)

HBS=: 0 : 0
jhh1 'WebGL 3d graphics'
'<h2>A Cube Lit by a Viewpoint Light</h2>'
'<input type="radio" name="projectionType" id="persproj" value="perspective" onchange="draw()"><label for="persproj">Perspective projection</label>'
'<input type="radio" name="projectionType" id="orthproj" value="orthogonal" onchange="draw()" style="margin-left:1cm"><label for="orthproj">Orthogonal projection</label>'
jhbr
jhbr
'bgcolor' jhb 'call J with ajax to get new background color'
'<p id=message>Use arrow keys to rotate the cube.  Home or return key sets rotations to zero.</p>'
'<canvas width=400 height=400 id="glcanvas"></canvas>'
jhdemo''
)

jev_get=: 3 : 0
'jdemo12'jhr''
)

ev_bgcolor_click=: 3 : 0
jhrajax ('[',(":255%~?255 255 255),']')rplc' ';','
)

CSS=: 0 : 0
body{margin:10px;}
)

JS=: 0 : 0 NB. javascript
function ev_body_load(){init();} // must use JHS framework load handler
function ev_bgcolor_click(){jdoajax([],"");}
function ev_bgcolor_click_ajax(ts){bgcolor=eval(ts[0]);draw()};
var bgcolor=[0,0,0];

// following directly from example on web
var gl;   // The webgl context.

var aCoords;           // Location of the coords attribute variable in the shader program.
var aCoordsBuffer;     // Buffer to hold coords.
var uColor;            // Location of the color uniform variable in the shader program.
var uProjection;       // Location of the projection uniform matrix in the shader program.
var uModelview;        // Location of the modelview unifirm matrix in the shader program.
var uNormal;           // Location of the normal uniform in the shader program.
var uLit;              // Location of the lit uniform in the shader program.
var uNormalMatrix;     // Location of the normalMatrix uniform matrix in the shader program.

var projection = mat4.create();   // projection matrix
var modelview = mat4.create();    // modelview matrix
var normalMatrix = mat3.create(); // matrix, derived from modelview matrix, for transforming normal vectors

var rotateX = 0.4;   // rotation of cube about the x-axis
var rotateY = -0.5;  // rotation of cube about the y-axis


/* Draws a WebGL primitive.  The first parameter must be one of the constants
 * that specifiy primitives:  gl.POINTS, gl.LINES, gl.LINE_LOOP, gl.LINE_STRIP,
 * gl.TRIANGLES, gl.TRIANGLE_STRIP, gl.TRIANGLE_FAN.  The second parameter must
 * be an array of 4 numbers in the range 0.0 to 1.0, giving the RGBA color of
 * the color of the primitive.  The third parameter must be an array of numbers.
 * The length of the array must be amultiple of 3.  Each triple of numbers provides
 * xyz-coords for one vertex for the primitive.  This assumes that uColor is the
 * location of a color uniform in the shader program, aCoords is the location of
 * the coords attribute, and aCoordsBuffer is a VBO for the coords attribute.
 */
function drawPrimitive( primitiveType, color, vertices ) {
     gl.enableVertexAttribArray(aCoords);
     gl.bindBuffer(gl.ARRAY_BUFFER,aCoordsBuffer);
     gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STREAM_DRAW);
     gl.uniform4fv(uColor, color);
     gl.vertexAttribPointer(aCoords, 3, gl.FLOAT, false, 0, 0);
     gl.drawArrays(primitiveType, 0, vertices.length/3);
}


/* Draws a colored cube, along with a set of coordinate axes.
 * (Note that the use of the above drawPrimitive function is not an efficient
 * way to draw with WebGL.  Here, the geometry is so simple that it doesn't matter.)
 */
function draw() { 
    gl.clearColor(bgcolor[0],bgcolor[1],bgcolor[2],1);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    
    if (document.getElementById("persproj").checked) {
         mat4.perspective(projection, Math.PI/4, 1, 4, 8);
    }
    else {
         mat4.ortho(projection,-2.5, 2.5, -2.5, 2.5, 4, 8);
    }
    gl.uniformMatrix4fv(uProjection, false, projection );

    mat4.lookAt(modelview, [0,0,6], [0,0,0], [0,1,0]);
    mat4.rotateX(modelview, modelview, rotateX);
    mat4.rotateY(modelview, modelview, rotateY);
    gl.uniformMatrix4fv(uModelview, false, modelview );

    mat3.normalFromMat4(normalMatrix, modelview);
    gl.uniformMatrix3fv(uNormalMatrix, false, normalMatrix);
    
    gl.uniform1i( uLit, 1 );  // Turn on lighting calculations for the cube.

    gl.uniform3f( uNormal, 0, 0, 1 );
    drawPrimitive( gl.TRIANGLE_FAN, [1,0,0,1], [ -1,-1,1, 1,-1,1, 1,1,1, -1,1,1 ]);
    gl.uniform3f( uNormal, 0, 0, -1 );
    drawPrimitive( gl.TRIANGLE_FAN, [0,1,0,1], [ -1,-1,-1, -1,1,-1, 1,1,-1, 1,-1,-1 ]);
    gl.uniform3f( uNormal, 0, 1, 0 );
    drawPrimitive( gl.TRIANGLE_FAN, [0,0,1,1], [ -1,1,-1, -1,1,1, 1,1,1, 1,1,-1 ]);
    gl.uniform3f( uNormal, 0, -1, 0 );
    drawPrimitive( gl.TRIANGLE_FAN, [1,1,0,1], [ -1,-1,-1, 1,-1,-1, 1,-1,1, -1,-1,1 ]);
    gl.uniform3f( uNormal, 1, 0, 0 );
    drawPrimitive( gl.TRIANGLE_FAN, [1,0,1,1], [ 1,-1,-1, 1,1,-1, 1,1,1, 1,-1,1 ]);
    gl.uniform3f( uNormal, -1, 0, 0 );
    drawPrimitive( gl.TRIANGLE_FAN, [0,1,1,1], [ -1,-1,-1, -1,-1,1, -1,1,1, -1,1,-1 ]);

    gl.uniform1i( uLit, 0 );  // The lines representing the coordinate axes are not lit.

    gl.lineWidth(4);
    drawPrimitive( gl.LINES, [1,0,0,1], [ -2,0,0, 2,0,0] );
    drawPrimitive( gl.LINES, [0,1,0,1], [ 0,-2,0, 0,2,0] );
    drawPrimitive( gl.LINES, [0,0,1,1], [ 0,0,-2, 0,0,2] );
    gl.lineWidth(1);
}

/* Creates a program for use in the WebGL context gl, and returns the
 * identifier for that program.  If an error occurs while compiling or
 * linking the program, an exception of type String is thrown.  The error
 * string contains the compilation or linking error.  If no error occurs,
 * the program identifier is the return value of the function.
 */
function createProgram(gl, vertexShaderSource, fragmentShaderSource) {
   var vsh = gl.createShader( gl.VERTEX_SHADER );
   gl.shaderSource(vsh,vertexShaderSource);
   gl.compileShader(vsh);
   if ( ! gl.getShaderParameter(vsh, gl.COMPILE_STATUS) ) {
      throw "Error in vertex shader:  " + gl.getShaderInfoLog(vsh);
   }
   var fsh = gl.createShader( gl.FRAGMENT_SHADER );
   gl.shaderSource(fsh, fragmentShaderSource);
   gl.compileShader(fsh);
   if ( ! gl.getShaderParameter(fsh, gl.COMPILE_STATUS) ) {
      throw "Error in fragment shader:  " + gl.getShaderInfoLog(fsh);
   }
   var prog = gl.createProgram();
   gl.attachShader(prog,vsh);
   gl.attachShader(prog, fsh);
   gl.linkProgram(prog);
   if ( ! gl.getProgramParameter( prog, gl.LINK_STATUS) ) {
      throw "Link error in program:  " + gl.getProgramInfoLog(prog);
   }
   return prog;
}


/**
 *  An event listener for the keydown event.  It is installed by the init() function.
 */
function doKey(evt) {
    var rotationChanged = true;
 switch (evt.keyCode) {
     case 37: rotateY -= 0.05; break;        // left arrow
     case 39: rotateY +=  0.05; break;       // right arrow
     case 38: rotateX -= 0.05; break;        // up arrow
     case 40: rotateX += 0.05; break;        // down arrow
     case 13: rotateX = rotateY = 0; break;  // return
     case 36: rotateX = rotateY = 0; break;  // home
     default: rotationChanged = false;
 }
 if (rotationChanged) {
           evt.preventDefault();
             draw();
 }
}


/* Gets the text content of an HTML element.  This is used
 * to get the shader source from the script elements that contain
 * it.  The parameter should be the id of the script element.
 */
function getTextContent( elementID ) {
    var element = document.getElementById(elementID);
    var fsource = "";
    var node = element.firstChild;
    var str = "";
    while (node) {
        if (node.nodeType == 3) // this is a text node
            str += node.textContent;
        node = node.nextSibling;
    }
    return str;
}


/**
 * Initializes the WebGL program including the relevant global variables
 * and the WebGL state.  Creates a SimpleView3D object for viewing the
 * cube and installs a mouse handler that lets the user rotate the cube.
 */
function init() {
   try {
        var canvas = document.getElementById("glcanvas");
        gl = canvas.getContext("webgl");
        if ( ! gl ) {
            gl = canvas.getContext("experimental-webgl");
        }
        if ( ! gl ) {
            throw "Could not create WebGL context.";
        }
        var vertexShaderSource = getTextContent("vshader"); 
        var fragmentShaderSource = getTextContent("fshader");
        var prog = createProgram(gl,vertexShaderSource,fragmentShaderSource);
        gl.useProgram(prog);
        aCoords =  gl.getAttribLocation(prog, "coords");
        uModelview = gl.getUniformLocation(prog, "modelview");
        uProjection = gl.getUniformLocation(prog, "projection");
        uColor =  gl.getUniformLocation(prog, "color");
        uLit =  gl.getUniformLocation(prog, "lit");
        uNormal =  gl.getUniformLocation(prog, "normal");
        uNormalMatrix =  gl.getUniformLocation(prog, "normalMatrix");
        aCoordsBuffer = gl.createBuffer();
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);  // no need to draw back faces
        document.getElementById("persproj").checked = true;
        document.addEventListener("keydown", doKey, false);   }
   catch (e) {
      document.getElementById("message").innerHTML =
           "Could not initialize WebGL: " + e;
      return;
   }
   draw();
}

)