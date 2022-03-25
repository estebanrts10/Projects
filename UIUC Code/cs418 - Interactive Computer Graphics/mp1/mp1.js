/**
 * @file A simple WebGL example drawing a triangle with colors
 * @author Eric Shaffer <shaffer1@eillinois.edu>
 *
 * Updated Spring 2021 to use WebGL 2.0 and GLSL 3.00
 */

/** @global The WebGL context */
var gl;

/** @global The HTML5 canvas we draw on */
var canvas;

/** @global A simple GLSL shader program */
var shaderProgram;

/** @global The WebGL buffer holding the triangle */
var vertexPositionBuffer;

/** @global The WebGL buffer holding the vertex colors */
var vertexColorBuffer;

/** @global The vertex array object for the triangle */
var vertexArrayObject;

/** @global The rotation angle of our triangle */
var rotAngle = 0;

/** @global this is temp */
var temp=0;

/** @global The ModelView matrix contains any modeling and viewing transformations */
var modelViewMatrix = glMatrix.mat4.create();

/** @global this holds the vertices globally */
var vertexArray;

/** @global this holds the vertices globally for my animation */
var myVertexArray;

/** @global this holds the colors globally */
var colorArray;

/** @global Records time last frame was rendered */
var previousTime = 0;

/** @global flag to check which animation is currently being displayed*/
var animFlag = 0;

/** @global the id*/
var animationFrame;

/**
 * Translates degrees to radians
 * @param {Number} degrees Degree input to function
 * @return {Number} The radians that correspond to the degree input
 */
function degToRad(degrees) {
        return degrees * Math.PI / 180;
}


/**
 * Creates a context for WebGL
 * @param {element} canvas WebGL canvas
 * @return {Object} WebGL context
 */
function createGLContext(canvas) {
  var context = null;
  context = canvas.getContext("webgl2");
  if (context) {
    context.viewportWidth = canvas.width;
    context.viewportHeight = canvas.height;
  } else {
    alert("Failed to create WebGL context!");
  }
  return context;
}


/**
 * Loads a shader.
 * Retrieves the source code from the HTML document and compiles it.
 * @param {string} id ID string for shader to load. Either vertex shader/fragment shader
 */
function loadShaderFromDOM(id) {
  var shaderScript = document.getElementById(id);

  // If we don't find an element with the specified id
  // we do an early exit
  if (!shaderScript) {
    return null;
  }

  var shaderSource = shaderScript.text;

  var shader;
  if (shaderScript.type == "x-shader/x-fragment") {
    shader = gl.createShader(gl.FRAGMENT_SHADER);
  } else if (shaderScript.type == "x-shader/x-vertex") {
    shader = gl.createShader(gl.VERTEX_SHADER);
  } else {
    return null;
  }

  gl.shaderSource(shader, shaderSource);
  gl.compileShader(shader);

  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    alert(gl.getShaderInfoLog(shader));
    return null;
  }
  return shader;
}


/**
 * Set up the fragment and vertex shaders.
 */
function setupShaders() {
  // Compile the shaders' source code.
  vertexShader = loadShaderFromDOM("shader-vs");
  fragmentShader = loadShaderFromDOM("shader-fs");

  // Link the shaders together into a program.
  shaderProgram = gl.createProgram();
  gl.attachShader(shaderProgram, vertexShader);
  gl.attachShader(shaderProgram, fragmentShader);
  gl.linkProgram(shaderProgram);

  if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
    alert("Failed to setup shaders");
  }

  // We only use one shader program for this example, so we can just bind
  // it as the current program here.
  gl.useProgram(shaderProgram);

  // Query the index of each attribute in the list of attributes maintained
  // by the GPU.
  shaderProgram.vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
  shaderProgram.vertexColorAttribute = gl.getAttribLocation(shaderProgram, "aVertexColor");

  //Get the index of the Uniform variable as well
  shaderProgram.modelViewMatrixUniform = gl.getUniformLocation(shaderProgram, "uModelViewMatrix");
}


/**
 * Set up the buffers to hold the triangle's vertex positions and colors.
 */
function setupBuffers(i) {

  // Create the vertex array object, which holds the list of attributes for
  // the triangle.
  vertexArrayObject = gl.createVertexArray();
  gl.bindVertexArray(vertexArrayObject);

  // Create a buffer for positions, and bind it to the vertex array object.
  vertexPositionBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, vertexPositionBuffer);

  // Define a triangle in clip coordinates.
  if(i==0 && animFlag == 1){
    getMyVertices();
  }
  //gets the different vertices for the different animations
  if(i==0 && animFlag == 0){
    getVertices();
  }

  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertexArray), gl.STATIC_DRAW);
  vertexPositionBuffer.itemSize = 3;
  if(animFlag == 1){
    vertexPositionBuffer.numberOfItems = 48;
  }
  else{
    vertexPositionBuffer.numberOfItems = 30;
  }
  // Binds the buffer that we just made to the vertex position attribute.
  gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute,
                         vertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);

  // Do the same steps for the color buffer.
  vertexColorBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, vertexColorBuffer);
  if(i==0 && animFlag == 1){
    getMyColors();
  }
  //gets the different colors for the different animations
  if(i==0 && animFlag == 0){
    getColors();
  }
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colorArray), gl.STATIC_DRAW);
  vertexColorBuffer.itemSize = 4;
  if(animFlag == 0){
    vertexColorBuffer.numberOfItems = 30;
  }
  else{
    vertexColorBuffer.numberOfItems = 48;
  }
  gl.vertexAttribPointer(shaderProgram.vertexColorAttribute,
                         vertexColorBuffer.itemSize, gl.FLOAT, false, 0, 0);

   // Enable each attribute we are using in the VAO.
  gl.enableVertexAttribArray(shaderProgram.vertexPositionAttribute);
  gl.enableVertexAttribArray(shaderProgram.vertexColorAttribute);

  // Unbind the vertex array object to be safe.
  gl.bindVertexArray(null);
}


/**
 * Draws a frame to the screen.
 */
function draw() {
  // Transform the clip coordinates so the render fills the canvas dimensions.
  gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);

  // Clear the screen.
  gl.clear(gl.COLOR_BUFFER_BIT);

  // Use the vertex array object that we set up.
  gl.bindVertexArray(vertexArrayObject);

  // Send the ModelView matrix with our transformations to the vertex shader.
  gl.uniformMatrix4fv(shaderProgram.modelViewMatrixUniform,
                      false, modelViewMatrix);

  // Render the triangle.
  gl.drawArrays(gl.TRIANGLES, 0, vertexPositionBuffer.numberOfItems);

  // Unbind the vertex array object to be safe.
  gl.bindVertexArray(null);
}
/**
  * This gives the I two affine transformations and the movement
  * using the array, which then moves with the buffer.
  */

function animationI(currentTime){
    var speed = document.getElementById("speed").value;

    // Convert the time to seconds.
    // Subtract the previous time from the current time.
    var deltaTime = currentTime - previousTime;
    // Remember the current time for the next frame.
    previousTime = currentTime;

    // Update geometry to rotate 'speed' degrees per second.
    rotAngle += speed * deltaTime;
    if (rotAngle > 360.0 && temp == 0){
        rotAngle = 0.0;
        temp = 1;
    }
    //if the I does one full rotation, then set temp flag to 1
    //this is to make the I rotate counterclockwise if it did a full clockwise
    //rotation and vice versa.
    if (rotAngle > 360 && temp == 1){
        rotAngle = 0.0;
        temp = 0;
    }
    var vec;
    var rotAngle2;
    if(temp == 1){
        rotAngle2 = 360 - rotAngle;
    }
    else{
        rotAngle2 = rotAngle;
    }
    var currentTime1 = currentTime;
    //this sets up the rotation of the I
    glMatrix.mat4.fromZRotation(modelViewMatrix, degToRad(rotAngle2));
    //this sets up the up and down scaling of the I
    if(currentTime%1.5>=.75){
      glMatrix.mat4.scale(modelViewMatrix,modelViewMatrix,[1.5-currentTime1%1.5,1.5-currentTime1%1.5,1.5-currentTime1%1.5]);
    }
    else{
      glMatrix.mat4.scale(modelViewMatrix,modelViewMatrix,[currentTime1%1.5,currentTime1%1.5,currentTime1%1.5]);
    }

    //this gives the I the movement that occurs
    if(currentTime%1.5>=.75){

      for(i=0; i<30; i++){
        vertexArray[i*3] -= .01;
        //vertexArray[i*3 + 1] -= .01;
      }
    }
    else{
      for(i=0; i<30; i++){
        vertexArray[i*3] += .01;
        //vertexArray[i*3 + 1] += .01;
      }
    }
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertexArray), gl.DYNAMIC_DRAW);
    return;
}
/**
  * changes the colors for each triangle in the color wheel, giving it the
  * look as if it is rotating.
  */
function myAnimation(currentTime){
    if((currentTime*2 % 1) < .01){
      for(i=0;i<48;i++){
        if(colorArray[i*4+1] == 120/255){ //if color is orange, set now to white
          colorArray[i*4+1] = 1.0;
          colorArray[i*4+2] = 1.0;
        }
        else if(colorArray[i*4+1] == 1.0){ //if color is white, set now to blue
          colorArray[i*4+1] = 25/255;
          colorArray[i*4] = 0.0;
          colorArray[i*4+2] = 200/255;
        }
        else{
          colorArray[i*4+1] = 120/255; //else (color is blue) set color to orange to finish cycle
          colorArray[i*4] = 1.0;
          colorArray[i*4+2] = 0.0;
        }
      }
    }
    return;
}
/**
 * Animates the triangle by updating the ModelView matrix with a rotation
 * each frame.
 */
 function animate(currentTime) {
   //keep track of time in animate
  // Read the speed slider from the web page.
  currentTime *= 0.0005;
  if(animFlag == 1){
    myAnimation(currentTime);
  }
  //if the animation flag is 1 then the colorwheel displays, otherwise the moving I
  else{
    animationI(currentTime);
  }

  setupBuffers(1);

  // Draw the frame.
  draw();

  // Animate the next frame. The animate function is passed the current time in
  // milliseconds.
  animationFrame = requestAnimationFrame(animate);

}
/**
  * If already in myAnimation then it does nothing, else changes the flag
  * and resets the buffers to start anew.
  */
function changeToMyAnimation(){
    if(animFlag == 1){
      return;
    }
    animFlag = 1;
    draw();
    cancelAnimationFrame(animationFrame);
    glMatrix.mat4.identity(modelViewMatrix);
    startup();


}
/**
  * If already in I animation then it does nothing, else changes the flag
  * and resets the buffers to start anew.
  */
function changeToI(){
    if(animFlag == 0){
      return;
    }
    animFlag = 0;
    draw();
    cancelAnimationFrame(animationFrame);
    glMatrix.mat4.identity(modelViewMatrix);
    startup();
}
/**
  * sets the vertices for every triangle needed to display the I
  */
function getVertices(){
  vertices = [

    -0.6,  1.0, 0.0,
    -0.6,  0.6, 0.0,
    -0.2,  0.6, 0.0, //2
    -0.2, -0.6, 0.0,
    -0.6, -0.6, 0.0,
    -0.6, -1.0, 0.0, // 5
    0.6, 1.0, 0.0,
    0.6, 0.6, 0.0,
    0.2, 0.6, 0.0, //4
    0.2, -0.6, 0.0,
    0.6,  -0.6, 0.0,
    0.6,  -1.0, 0.0, //8
    -0.2,  0.6, 0.0,
    -0.6,  1.0, 0.0,
    0.6, 1.0, 0.0, //1
    -0.2, 0.6, 0.0,
    0.2, 0.6, 0.0,
    0.6, 1.0, 0.0, //3
    -0.2, -0.6, 0.0,
    0.2, -0.6, 0.0,
    0.6, -1.0, 0.0, //7
    -0.2,  -0.6, 0.0,
    -0.6,  -1.0, 0.0,
    0.6, -1.0, 0.0, //6
    -0.2, -0.6, 0.0,
    0.2, -0.6, 0.0,
    -0.2, 0.6, 0.0, //9
    -0.2, 0.6, 0.0,
    0.2, 0.6, 0.0,
    0.2, -0.6, 0.0,// 10

  ];
  //once the vertices are set, they are then placed in the global array.
  vertexArray = vertices;
  return;
}
/**
  * sets the color for each triangle that is used in the I (orange)
  */
function getColors(){

      var color = [
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
    ];
    //once the colors are set, they are then placed in the global array.
    colorArray = color;
    return;
}
/**
  * sets the vertices for my animation
  */
function getMyVertices(){

      var vertices = [
        -1,1,0,
        -.5,1,0,
        0,0,0,//1
        -.5,1,0,
        0,1,0,
        0,0,0,//2
        0,1,0,
        .5,1,0,
        0,0,0,//3
        .5,1,0,
        1,1,0,
        0,0,0,//4
        1,1,0,
        1,.5,0,
        0,0,0,//5
        1,.5,0,
        1,0,0,
        0,0,0,//6
        1,0,0,
        1,-.5,0,
        0,0,0,//7
        1,-.5,0,
        1,-1,0,
        0,0,0,//8
        1,-1,0,
        .5,-1,0,
        0,0,0,//9
        .5,-1,0,
        0,-1,0,
        0,0,0,//10
        0,-1,0,
        -.5,-1,0,
        0,0,0,//11
        -.5,-1,0,
        -1,-1,0,
        0,0,0,//12
        -1,-1,0,
        -1,-.5,0,
        0,0,0,//13
        -1,-.5,0,
        -1,0,0,
        0,0,0,//14
        -1,0,0,
        -1,.5,0,
        0,0,0,//15
        -1,.5,0,
        -1,1,0,
        0,0,0,//16
      ];
      //once the vertices are set, the colors are then placed in the global array.
      vertexArray = vertices;
      return;
}
/**
  * sets the colors for each triangle in my animation
  */
function getMyColors(){


      var color = [
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        0.0, 25/255, 200/255, 1.1,
        0.0, 25/255, 200/255, 1.1,
        0.0, 25/255, 200/255, 1.1,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        0.0, 25/255, 200/255, 1.1,
        0.0, 25/255, 200/255, 1.1,
        0.0, 25/255, 200/255, 1.1,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        0.0, 25/255, 200/255, 1.1,
        0.0, 25/255, 200/255, 1.1,
        0.0, 25/255, 200/255, 1.1,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        0.0, 25/255, 200/255, 1.1,
        0.0, 25/255, 200/255, 1.1,
        0.0, 25/255, 200/255, 1.1,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 120/255, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        0.0, 25/255, 200/255, 1.1,
        0.0, 25/255, 200/255, 1.1,
        0.0, 25/255, 200/255, 1.1,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,

    ];
    //once the colors are set, they are then input in the global array
    colorArray = color;
    return;


}

/**
 * Startup function called from html code to start the program.
 */
 function startup() {
  console.log("Starting animation...");
  canvas = document.getElementById("myGLCanvas");
  gl = createGLContext(canvas);
  setupShaders();
  setupBuffers(0);
  gl.clearColor(1.0, 1.0, 0.8, 1.0);
  animationFrame = requestAnimationFrame(animate);
}
