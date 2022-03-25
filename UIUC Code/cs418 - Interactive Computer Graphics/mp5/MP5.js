/**
 * @file MP5.js - A simple WebGL rendering engine
 * @author Ian Rudnick <itr2@illinois.edu>
 * @brief Starter code for CS 418 MP5 at the University of Illinois at
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

/** @global An object holding the geometry for your 3D model */
var sphere1;

/** @global The Model matrix */
var modelViewMatrix = glMatrix.mat4.create();
/** @global The Model matrix */
var viewMatrix = glMatrix.mat4.create();
/** @global The Projection matrix */
var projectionMatrix = glMatrix.mat4.create();
/** @global The Normal matrix */
var normalMatrix = glMatrix.mat3.create();

// Material parameters
/** @global Ambient material color/intensity for Phong reflection */
var kAmbient = [0.25, 0.75, 1.0];
/** @global Diffuse material color/intensity for Phong reflection */
var kDiffuse = [0.25, 0.75, 1.0];
/** @global Specular material color/intensity for Phong reflection */
var kSpecular = [0.25, 0.75, 1.0];
/** @global Shininess exponent for Phong reflection */
var shininess = 2;

/** @global Ambient light color */
const lAmbient = [0.4, 0.4, 0.4];
/** @global Diffuse light color */
const lDiffuse = [1.0, 1.0, 1.0];
/** @global Specular  light color */
const lSpecular = [1.0, 1.0, 1.0];

var addSpheres = 0;
/** @global all arrays holding necessary information about spheres */
var spherePosArray = [];
var sphereVelArray = [];
var numSpheres = 5;
var sphereColArray = [];
var sphereRadArray = [];
/**
 * Translates degrees to radians
 * @param {Number} degrees Degree input to function
 * @return {Number} The radians that correspond to the degree input
 */
function degToRad(degrees) {
  return degrees * Math.PI / 180;
}

//-----------------------------------------------------------------------------
// Setup functions (run once when the webpage loads)
/**
 * Startup function called from the HTML code to start program.
 */
function startup() {
  // Set up the canvas with a WebGL context.
  canvas = document.getElementById("glCanvas");
  gl = createGLContext(canvas);

  // Compile and link a shader program.
  setupShaders();

//  document.onkeydown = keyDown;
//  document.onkeyup = keyUp;


  // Create a sphere mesh and set up WebGL buffers for it.
  sphere1 = new Sphere(5);
  sphere1.setupBuffers(shaderProgram);
  for(var i=0; i<numSpheres; i++){
    spherePosArray.push(glMatrix.vec3.create());
  }
  setupSpheres();


  // Create the projection matrix with perspective projection.
  const near = 0.1;
  const far = 200.0;
  glMatrix.mat4.perspective(projectionMatrix, degToRad(45),
                            gl.viewportWidth / gl.viewportHeight,
                            near, far);

  // Set the background color to black (you can change this if you like).
  gl.clearColor(0.0, 0.0, 0.0, 1.0);
  gl.enable(gl.DEPTH_TEST);
  gl.enable(gl.CULL_FACE);
  //gl.frontFace(gl.CW);

  // Start animating.
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

  // If you have multiple different shader programs, you'll need to move this
  // function to draw() and call it whenever you want to switch programs
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
  shaderProgram.locations.specularLightColor =
  gl.getUniformLocation(shaderProgram, "specularLightColor");
}

//-----------------------------------------------------------------------------
// Animation functions (run every frame)
/**
 * Draws the current frame and then requests to draw the next frame.
 * @param {number} currentTime The elapsed time in milliseconds since the
 *    webpage loaded.
 */
function animate(currentTime) {
  // Add code here using currentTime if you want to add animations

  // Set up the canvas for this frame
  gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  var modelMatrix = glMatrix.mat4.create();
  var viewMatrix = glMatrix.mat4.create();

  // Create the view matrix using lookat.
  const lookAtPt = glMatrix.vec3.fromValues(0.0, 0.0, 0.0);
  const eyePt = glMatrix.vec3.fromValues(0.0, 0.0, 10.0);
  const up = glMatrix.vec3.fromValues(0.0, 1.0, 0.0);
  glMatrix.mat4.lookAt(viewMatrix, eyePt, lookAtPt, up);

  // Concatenate the model and view matrices.
  // Remember matrix multiplication order is important.
  //glMatrix.mat4.multiply(modelViewMatrix, viewMatrix, modelMatrix);

  //setMatrixUniforms();

  // Transform the light position to view coordinates
  var lightPosition = glMatrix.vec3.fromValues(5, 5, -5);
  glMatrix.vec3.transformMat4(lightPosition, lightPosition, viewMatrix);

  setLightUniforms(lAmbient, lDiffuse, lSpecular, lightPosition);
  //setMaterialUniforms(kAmbient, kDiffuse, kSpecular, shininess);

  // You can draw multiple spheres by changing the modelViewMatrix, calling
  // setMatrixUniforms() again, and calling gl.drawArrays() again for each
  // sphere. You can use the same sphere object and VAO for all of them,
  // since they have the same triangle mesh.

  var e, v, tempMat;
  // sphere1.bindVAO();

    for(var i=0; i<numSpheres; i++){
      e = Math.random();
      v = Math.random();
      updatePosVel(i);
      //tempMat = glMatrix.vec3.fromValues(e,e,e);
      matV = glMatrix.mat4.create();
      matE = glMatrix.mat4.create();
      glMatrix.mat4.scale(matE,matE,sphereRadArray[i]);
      //tempMat = glMatrix.vec3.fromValues(Math.random(),Math.random(),Math.random());
      //could try different velocities for each direction
      glMatrix.mat4.translate(matV,matV,spherePosArray[i]);
      glMatrix.mat4.multiply(modelMatrix,matV,matE);
      glMatrix.mat4.multiply(modelViewMatrix,viewMatrix,modelMatrix);
      kDiffuse = sphereColArray[i];
      setMaterialUniforms(kAmbient, kDiffuse, kSpecular, shininess);
      setMatrixUniforms();
      // drawSphere();
      sphere1.bindVAO();
      gl.drawArrays(gl.TRIANGLES, 0, sphere1.numTriangles*3);
      sphere1.unbindVAO();
    }

  // sphere1.unbindVAO();


  requestAnimationFrame(animate);
}


//-----------------------------------------------------------------------------
// change number of spheres functions (run every frame)
/**
 * Depending on which button is pressed, either removes all spheres or adds 5
 * @param add add is either 5 or 0 flag is 0 or 1 depending on if we want to remove all spheres
 */
function changeSphere(add,flag){
  if(flag){
    numSpheres=0;
    sphereRadArray = [];
    sphereColArray=[];
    sphereVelArray=[];
  }
  else{
    for(var i=0; i<add; i++){
      spherePosArray.push(glMatrix.vec3.create());
    }
    for(var i=numSpheres; i<numSpheres+add; i++){
      var temp = (Math.random()*.5 + .2);
      sphereRadArray[i] = glMatrix.vec3.fromValues(temp,temp,temp);
      //console.log(temp);
      sphereColArray[i] = glMatrix.vec3.fromValues(Math.random(),Math.random(),Math.random());
      sphereVelArray[i] = glMatrix.vec3.fromValues((Math.random()-.5)*4,(Math.random()-.5)*4,(Math.random()-.5)*4);
      //changes velocity values from [0,1] to [-2,2]
      // console.log(" ");
      // console.log(sphereVelArray[i]);
    }
    numSpheres+=add;
  }
  //requestAnimationFrame(animate);

}


//-----------------------------------------------------------------------------
// setupSpheres
/**
 * gives each sphere a random color, velocity and radius
 * @param idx number of current sphere
 */

function setupSpheres(idx){
  for(var i=0; i<numSpheres; i++){
    var temp = (Math.random()*.5 + .2);
    sphereRadArray[i] = glMatrix.vec3.fromValues(temp,temp,temp);
    //console.log(temp);
    sphereColArray[i] = glMatrix.vec3.fromValues(Math.random(),Math.random(),Math.random());
    sphereVelArray[i] = glMatrix.vec3.fromValues((Math.random()-.5)*4,(Math.random()-.5)*4,(Math.random()-.5)*4);
    //changes velocity values from [0,1] to [-2,2]
    // console.log(" ");
    // console.log(sphereVelArray[i]);
  }
}

//-----------------------------------------------------------------------------
// updatePosVel
/**
 * updates the velocity of the sphere and position given old velocity, old position, and gravity
 * @param idx number of current sphere
 */

function updatePosVel(idx){
  //given idx, global array with random positions and velocity
  var oldPos = spherePosArray[idx];
   //should be in form [0,1,0] already
  var oldVel = sphereVelArray[idx];
  var changeInPos = glMatrix.vec3.create();
  // changeInPos[0] = oldVel[0]*0.15;
  // changeInPos[1] = oldVel[0]*0.15;
  // changeInPos[2] = oldVel[0]*0.15;
  glMatrix.vec3.scale(changeInPos,oldVel,0.15);
  var newPos = glMatrix.vec3.create();
  glMatrix.vec3.add(newPos, oldPos, changeInPos);
  spherePosArray[idx] = newPos;
  //adding cube bounds and reflection of velocities
  if(newPos[0]>=2 || newPos[0] <= -2){
    sphereVelArray[idx][0] = -sphereVelArray[idx][0];
  }
  if(newPos[1]>=2 || newPos[1] <= -2){
    sphereVelArray[idx][1] = -sphereVelArray[idx][1];
  }
  if(newPos[2]>=2 || newPos[2] <= -2){
    sphereVelArray[idx][2] = -sphereVelArray[idx][2];
  }

  //console.log(sphereVelArray[idx]);
  if(spherePosArray[idx][1] <= -2 && (sphereVelArray[idx][1]) < 0.0005){
    sphereVelArray[idx] = glMatrix.vec3.fromValues(0,0,0);
    //clamping
  }
  else{
    //else normal movement with drag and acceleration
    var drag = .8;
    glMatrix.vec3.scale(oldVel,oldVel,Math.pow(drag,0.1));
    var acceleration = glMatrix.vec3.create();
    glMatrix.vec3.scale(acceleration, [0,-5,0],0.01);
    glMatrix.vec3.add(oldVel, acceleration, oldVel);
    sphereVelArray[idx] = oldVel;
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
