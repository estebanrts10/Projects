/**
 * @file MP3.js - A simple WebGL rendering engine
 * @author Ian Rudnick <itr2@illinois.edu>
 * @brief Starter code for CS 418 MP3 at the University of Illinois at
 * Urbana-Champaign.
 *
 * Updated Spring 2021 for WebGL 2.0/GLSL 3.00 ES.
 */

/** @global The WebGL context */
var gl;

/** @global The HTML5 canvas to draw on */
var canvas;

/** @global The GLSL shader program */
var shaderProgram;

/** @global An object holding the geometry for your 3D terrain */
var myTerrain;

/** @global The Model matrix */
var modelViewMatrix = glMatrix.mat4.create();
/** @global The Projection matrix */
var projectionMatrix = glMatrix.mat4.create();
/** @global The Normal matrix */
var normalMatrix = glMatrix.mat3.create();

/** @global camera current position vec3 */
var camPosition = glMatrix.vec3.fromValues(0.0, -2.0, 2);
/** @global camera current orientation quaternion */
var camOrientation = glMatrix.quat.create();
/** @global camera current speed in the forward direction */
var camSpeed = 0.005;
/** @global camera initial view direction */
var camInitialDir = glMatrix.vec3.fromValues(0.0, 0.5, -.5);
var forwardDirection = glMatrix.vec3.create();

/** @global The currently pressed keys */
var keys = {};

// Material parameters
/** @global Ambient material color/intensity for Phong reflection */
var kAmbient = [227/255, 191/255, 76/255];
/** @global Diffuse material color/intensity for Phong reflection */
var kDiffuse = [227/255, 191/255, 76/255];
/** @global Specular material color/intensity for Phong reflection */
var kSpecular = [227/255, 191/255, 76/255];
/** @global Shininess exponent for Phong reflection */
var shininess = 8;

/** @global look at point */
var lookAtPt = glMatrix.vec3.fromValues(0.3, 0.5, -.5);
/** @global eyepoint */
var eyePt = glMatrix.vec3.fromValues(0.0, -2.0, 2.0);
/** @global up vector */
var up = glMatrix.vec3.fromValues(0.0, 1.0, 0.0);
/** @global up vector */
var center = glMatrix.vec3.fromValues(0.3, 0.5, -.5);
/** @global up vector */
var newUp = glMatrix.vec3.fromValues(0.0, 1.0, 0.0);

// Light parameters
/** @global Light position in VIEW coordinates */
var lightPosition = [0, 2, 2];
/** @global Ambient light color/intensity for Phong reflection */
var ambientLightColor = [0.1, 0.1, 0.1];
/** @global Diffuse light color/intensity for Phong reflection */
var diffuseLightColor = [1, 1, 1];
/** @global Specular light color/intensity for Phong reflection */
var specularLightColor = [1, 1, 1];

/** @global Edge color for black wireframe */
var kEdgeBlack = [0.0, 0.0, 0.0];
/** @global Edge color for white wireframe */
var kEdgeWhite = [0.7, 0.7, 0.7];

/**
 * Translates degrees to radians
 * @param {Number} degrees Degree input to function
 * @return {Number} The radians that correspond to the degree input
 */
function degToRad(degrees) {
  return degrees * Math.PI / 180;
}


//-----------------------------------------------------------------------------
// Setup functions (run once)
/**
 * Startup function called from the HTML code to start program.
 */
function startup() {
  // Set up the canvas with a WebGL context.
  canvas = document.getElementById("glCanvas");
  gl = createGLContext(canvas);

  // Compile and link the shader program.
  setupShaders();

  // Let the Terrain object set up its own buffers.
  myTerrain = new Terrain(100, -1, 1, -1, 1);
  myTerrain.setupBuffers(shaderProgram);

  // Set the background color to sky blue (you can change this if you like).
  gl.clearColor(1.0,1.0,1.0,1.0);
  //gl.clearColor(0.82, 0.93, 0.99, 1.0);
  gl.enable(gl.DEPTH_TEST);

  document.onkeydown = keyDown;
  document.onkeyup = keyUp;

  requestAnimationFrame(animate);
}


/**
 * Creates a WebGL 2.0 context.
 * @param {element} canvas The HTML5 canvas to attach the context to.
 * @return {Object} The WebGL 2.0 context.
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
 * Loads a shader from the HTML document and compiles it.
 * @param {string} id ID string of the shader script to load.
 */
function loadShaderFromDOM(id) {
  var shaderScript = document.getElementById(id);

  // Return null if we don't find an element with the specified id
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
 * Sets up the vertex and fragment shaders.
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

  // We only need the one shader program for this rendering, so we can just
  // bind it as the current program here.
  gl.useProgram(shaderProgram);

  // Query the index of each attribute and uniform in the shader program.
  shaderProgram.locations = {};
  shaderProgram.locations.vertexPosition =
    gl.getAttribLocation(shaderProgram, "vertexPosition");
  shaderProgram.locations.vertexNormal =
    gl.getAttribLocation(shaderProgram, "vertexNormal");

  shaderProgram.locations.modelViewMatrix =
    gl.getUniformLocation(shaderProgram, "modelViewMatrix");
  shaderProgram.locations.projectionMatrix =
    gl.getUniformLocation(shaderProgram, "projectionMatrix");
  shaderProgram.locations.normalMatrix =
    gl.getUniformLocation(shaderProgram, "normalMatrix");

  shaderProgram.locations.kAmbient =
    gl.getUniformLocation(shaderProgram, "kAmbient");
  shaderProgram.locations.kDiffuse =
    gl.getUniformLocation(shaderProgram, "kDiffuse");
  shaderProgram.locations.kSpecular =
    gl.getUniformLocation(shaderProgram, "kSpecular");
  shaderProgram.locations.shininess =
    gl.getUniformLocation(shaderProgram, "shininess");

  shaderProgram.locations.lightPosition =
    gl.getUniformLocation(shaderProgram, "lightPosition");
  shaderProgram.locations.ambientLightColor =
    gl.getUniformLocation(shaderProgram, "ambientLightColor");
  shaderProgram.locations.diffuseLightColor =
  gl.getUniformLocation(shaderProgram, "diffuseLightColor");
  shaderProgram.locations.specularLightColor = gl.getUniformLocation(shaderProgram, "specularLightColor");
  shaderProgram.maxZ = gl.getUniformLocation(shaderProgram, "highZ");
  shaderProgram.minZ = gl.getUniformLocation(shaderProgram, "lowZ");
  shaderProgram.fogFlag = gl.getUniformLocation(shaderProgram,"flag");
}

/**
 * Draws the terrain to the screen.
 */
function draw() {
  // Transform the clip coordinates so the render fills the canvas dimensions.
  gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
  // Clear the color buffer and the depth buffer.
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  // Generate the projection matrix using perspective projection.
  const near = 0.1;
  const far = 200.0;
  glMatrix.mat4.perspective(projectionMatrix, degToRad(45),
                            gl.viewportWidth / gl.viewportHeight,
                            near, far);

  // Generate the view matrix using lookat.
  //glMatrix.mat4.lookAt(modelViewMatrix, eyePt, lookAtPt, newUp);
  glMatrix.mat4.lookAt(modelViewMatrix,camPosition,center,newUp);
  setMatrixUniforms();
  setLightUniforms(ambientLightColor, diffuseLightColor, specularLightColor,
                   lightPosition);
  //getHighLowZ();
  // Draw the triangles, the wireframe, or both, based on the render selection.
  if (document.getElementById("polygon").checked) {
    setMaterialUniforms(kAmbient, kDiffuse, kSpecular, shininess);
    myTerrain.drawTriangles();
  }
  else if (document.getElementById("wirepoly").checked) {
    setMaterialUniforms(kAmbient, kDiffuse, kSpecular, shininess);
    gl.enable(gl.POLYGON_OFFSET_FILL);
    gl.polygonOffset(1, 1);
    myTerrain.drawTriangles();
    gl.disable(gl.POLYGON_OFFSET_FILL);
    setMaterialUniforms(kEdgeBlack, kEdgeBlack, kEdgeBlack, shininess);
    myTerrain.drawEdges();
  }
  else if (document.getElementById("wireframe").checked) {
    setMaterialUniforms(kEdgeBlack, kEdgeBlack, kEdgeBlack, shininess);
    myTerrain.drawEdges();
  }
}


/**
 * Sends the three matrix uniforms to the shader program.
 */
function setMatrixUniforms() {
  gl.uniformMatrix4fv(shaderProgram.locations.modelViewMatrix, false,
                      modelViewMatrix);
  gl.uniformMatrix4fv(shaderProgram.locations.projectionMatrix, false,
                      projectionMatrix);

  // We want to transform the normals by the inverse-transpose of the
  // Model/View matrix
  glMatrix.mat3.fromMat4(normalMatrix,modelViewMatrix);
  glMatrix.mat3.transpose(normalMatrix,normalMatrix);
  glMatrix.mat3.invert(normalMatrix,normalMatrix);

  gl.uniformMatrix3fv(shaderProgram.locations.normalMatrix, false,
                      normalMatrix);
}


/**
 * Sends material properties to the shader program.
 * @param {Float32Array} a Ambient material color.
 * @param {Float32Array} d Diffuse material color.
 * @param {Float32Array} s Specular material color.
 * @param {Float32} alpha shininess coefficient
 */
function setMaterialUniforms(a, d, s, alpha) {
  gl.uniform3fv(shaderProgram.locations.kAmbient, a);
  gl.uniform3fv(shaderProgram.locations.kDiffuse, d);
  gl.uniform3fv(shaderProgram.locations.kSpecular, s);
  gl.uniform1f(shaderProgram.locations.shininess, alpha);
  getHighLowZ();
  getFogFlag();
}
/**
 * Sends material properties to the shader program.
 *
 *

 */
function getHighLowZ(){
  var high,low
  [high,low] = myTerrain.getHighAndLow();
  //console.log(high);
  //console.log(low);
  gl.uniform1f(shaderProgram.maxZ,high);
  gl.uniform1f(shaderProgram.minZ,low);
  //return;
}
function getFogFlag(){
  if (document.getElementById("fog").checked) {
    gl.uniform1i(shaderProgram.fogFlag, 1);
  } else {
    gl.uniform1i(shaderProgram.fogFlag, 0);
  }
}
/**
 * Sends light information to the shader program.
 * @param {Float32Array} a Ambient light color/intensity.
 * @param {Float32Array} d Diffuse light color/intensity.
 * @param {Float32Array} s Specular light color/intensity.
 * @param {Float32Array} loc The light position, in view coordinates.
 */
function setLightUniforms(a, d, s, loc) {
  gl.uniform3fv(shaderProgram.locations.ambientLightColor, a);
  gl.uniform3fv(shaderProgram.locations.diffuseLightColor, d);
  gl.uniform3fv(shaderProgram.locations.specularLightColor, s);
  gl.uniform3fv(shaderProgram.locations.lightPosition, loc);
}


/**
 * Animates...allows user to change the geometry view between
 * wireframe, polgon, or both.
 */
 function animate() {
    var eulerX = 0; //pitch
    var eulerZ = 0; //roll
    var eulerY = 0;

    //roll is rotation around the local Z axis
    //pitch is rotation around the local X axis

    if (keys["ArrowLeft"])
      eulerY += 1;
    if(keys["ArrowRight"])
      eulerY -= 1;
    if(keys["ArrowDown"])
      eulerX -= 1;
      //keyDown(something);
    if(keys["ArrowUp"])
      eulerX += 1;
    if(keys["Escape"]){
      restart();
    }

    if(keys["="]) {
      camSpeed += 0.001;
    }
    if (keys["-"]){
      camSpeed -= 0.001;
    }
      //keyUp(something);
    orientationDelta = glMatrix.quat.create();
    glMatrix.quat.fromEuler(orientationDelta,eulerX,eulerY,eulerZ);
    glMatrix.quat.multiply(camOrientation, camOrientation, orientationDelta);
    //might have to swap camOrientation and orientationDelta
    //updating camPosition by calculating a displacement vector deltaPosition
    //in the forward direction fo the view and adding it to camPosition
    //camPosition = vec3  create deltaposition caminitial = vec3
    //camOrientation
    deltaPosition = glMatrix.vec3.create();
    glMatrix.vec3.transformQuat(forwardDirection, camInitialDir, camOrientation);
    glMatrix.vec3.normalize(forwardDirection,forwardDirection);

    glMatrix.vec3.scale(deltaPosition,forwardDirection,camSpeed);
    glMatrix.vec3.add(camPosition,camPosition,deltaPosition);

    //upTemp = glMatrix.vec3.create();
    // (0.0,1.0,0.0) is up vector
    //newUp = glMatrix.vec3.create();
    glMatrix.vec3.transformQuat(newUp,up,camOrientation);
    glMatrix.vec3.transformQuat(center,camInitialDir,camOrientation);
    glMatrix.vec3.add(center,center,camPosition);
    //glMatrix.mat4.lookAt(modelViewMatrix,camPosition,center,newUp);
    //modelViewMatrix, eyePt, lookAtPt, up

    // Draw the frame.
    draw();
    // Animate the next frame.




    requestAnimationFrame(animate);
}

/**
 * Reinitializes every global variable used and returns, so that it
 * restarts at the original spot and speed.
 */
function restart(){
  center = glMatrix.vec3.fromValues(0.3, 0.5, -.5);
  newUp = glMatrix.vec3.fromValues(0.0, 1.0, 0.0);
  camPosition = glMatrix.vec3.fromValues(0.0, -2.0, 2);
  camOrientation = glMatrix.quat.create();
  camSpeed = 0.005;
  camInitialDir = glMatrix.vec3.fromValues(0.0, 0.5, -.5);
  forwardDirection = glMatrix.vec3.create();
  glMatrix.vec3.fromValues(0.3, 0.5, -.5);
  glMatrix.vec3.fromValues(0.0, -2.0, 2.0);
  glMatrix.vec3.fromValues(0.0, 1.0, 0.0);
  return;
}

/**
 * the handler for when a key is pressed.
 */
function keyDown(event) {
  if(event.key == "ArrowDown" || event.key == "ArrowUp" || event.key == "ArrowLeft" || event.key == "ArrowRight"){
    event.preventDefault();
  }
  keys[event.key] = true;
}

/**
 * handler for when a key is released.
 */
function keyUp(event) {
  keys[event.key] = false;
}
