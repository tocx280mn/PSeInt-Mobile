
plugins {
  alias(libs.plugins.android.application)
  alias(libs.plugins.kotlin.compose)
  alias(libs.plugins.roborazzi)
}

// Generate a portable build copy; never rewrite the bundled desktop sources.
// Octal byte escapes preserve PSeInt's original 8-bit literals with Clang/UTF-8.
abstract class PreparePseintCore : DefaultTask() {
  @get:InputDirectory @get:PathSensitive(PathSensitivity.RELATIVE)
  abstract val desktopCore: DirectoryProperty
  @get:OutputDirectory abstract val generatedCore: DirectoryProperty
  @get:OutputDirectory abstract val referenceCore: DirectoryProperty
  private fun writeIfChanged(file: java.io.File, text: String) {
    if (!file.exists() || file.readText() != text) file.writeText(text)
  }
  @TaskAction fun generate() {
    val destination = generatedCore.get().asFile.apply { mkdirs() }
    val reference = referenceCore.get().asFile.apply { mkdirs() }
    desktopCore.get().asFile.listFiles()!!.filter { it.extension in listOf("cpp", "h", "hpp") }.forEach { original ->
      var code = original.readBytes().toString(Charsets.ISO_8859_1).replace("\r\n", "\n")
      writeIfChanged(reference.resolve(original.name), buildString {
        code.forEach { char -> if (char.code >= 128) append("\\" + char.code.toString(8).padStart(3, '0')) else append(char) }
      })
      fun patch(before: String, after: String) {
        check(code.contains(before)) { "Desktop adapter anchor missing in ${original.name}: $before" }
        code = code.replace(before, after)
      }
      when (original.name) {
        "intercambio.cpp" -> {
          patch("void Intercambio::ChatWithGUI () {", "void Intercambio::ChatWithGUI () {\n mobileStep();")
          patch("void Intercambio::SetLocation(CodeLocation _loc) {", "void Intercambio::SetLocation(CodeLocation _loc) {\n mobileCheckpoint();")
        }
        "Ejecutar.cpp" -> {
          patch("void Ejecutar(RunTime &rt, int LineStart, int LineEnd) {", "void Ejecutar(RunTime &rt, int LineStart, int LineEnd) {\n MobileDepthGuard depthGuard;")
          patch("aux1=getLine();", "aux1=mobileRead(variable);")
          patch("Sleep(time.GetAsInt()*factor)", "mobileSleep(static_cast<long long>(time.GetAsInt())*factor)")
        }
        "new_memoria.h" -> {
          patch("void ListVars(", "const std::map<std::string,tipo_var> &MobileVariableNames() const { return var_info; }\n\tvoid ListVars(")
          // Original recursion walks beyond the dimensions array after erasing a
          // cell. It also uses scalar delete on an array and assumes base 1.
          patch("if (cur++==odims[0]) \n\t\t\tvar_value.erase(nombre+\")\");", "if (cur++==odims[0]) { var_value.erase(nombre+\")\"); return; }")
          patch("std::to_string(i+1),odims", "std::to_string(i+mobileArrayBase()),odims")
          patch("delete v.dims;", "delete [] v.dims;")
        }
        "Evaluar.cpp" -> patch("if (int(ret.GetAsReal())!=idx)", "if (ret.GetAsReal()!=idx)")
        "SynCheck.cpp" -> {
          patch("inst.setType(IT_PARACADA);", "if (!lang[LS_ALLOW_FOR_EACH]) err_handler.SyntaxError(1001,\"Este perfil no permite Para Cada.\");\n\t\t\t\tinst.setType(IT_PARACADA);")
          patch("getImpl<IT_HASTAQUE>(inst).mientras_que = true;", "if (!lang[LS_ALLOW_REPEAT_WHILE]) err_handler.SyntaxError(1002,\"Este perfil no permite Repetir Mientras Que.\");\n\t\t\t\tgetImpl<IT_HASTAQUE>(inst).mientras_que = true;")
          patch("getImpl<IT_DIMENSION>(inst).redimension = first_word_id==KW_REDIMENSIONAR;", "if (first_word_id==KW_REDIMENSIONAR && !lang[LS_ALLOW_RESIZE_ARRAYS]) err_handler.SyntaxError(1003,\"Este perfil no permite redimensionar arreglos.\");\n\t\t\t\tgetImpl<IT_DIMENSION>(inst).redimension = first_word_id==KW_REDIMENSIONAR;")
        }
      }
      code = buildString {
        code.forEach { char -> if (char.code >= 128) append("\\" + char.code.toString(8).padStart(3, '0')) else append(char) }
      }
      if (original.name in listOf("intercambio.cpp", "Ejecutar.cpp", "new_memoria.h")) code = "#include \"mobile_runtime.h\"\n" + code
      writeIfChanged(destination.resolve(original.name), code)
    }
  }
}
val preparePseintCore by tasks.registering(PreparePseintCore::class) {
  desktopCore.set(rootProject.layout.projectDirectory.dir("pseint/pseint"))
  generatedCore.set(layout.buildDirectory.dir("generated/pseintCore"))
  referenceCore.set(layout.buildDirectory.dir("generated/pseintReference"))
}
tasks.configureEach {
  if (name.startsWith("configureCMake")) dependsOn(preparePseintCore)
}
tasks.withType<Test>().configureEach {
  systemProperty("pseint.native.library", layout.buildDirectory.file("native-host/${if (System.getProperty("os.name").startsWith("Windows")) "libpseint.dll" else "libpseint.so"}").get().asFile.absolutePath)
}

android {
  namespace = "com.example"
  ndkVersion = "28.2.13676358"
  externalNativeBuild {
    cmake {
      path = file("src/main/cpp/CMakeLists.txt")
      version = "3.22.1"
    }
  }
  compileSdk { version = release(36) { minorApiLevel = 1 } }

  defaultConfig {
    applicationId = "com.aistudio.pseint.kjlmn"
    minSdk = 24
    targetSdk = 36
    versionCode = 1
    versionName = "1.0"
    ndk { abiFilters += listOf("arm64-v8a", "armeabi-v7a", "x86_64") }

    testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
  }

  signingConfigs {
    create("release") {
      val keystorePath = System.getenv("KEYSTORE_PATH") ?: "${rootDir}/my-upload-key.jks"
      storeFile = file(keystorePath)
      storePassword = System.getenv("STORE_PASSWORD")
      keyAlias = "upload"
      keyPassword = System.getenv("KEY_PASSWORD")
    }
  }

  buildTypes {
    release {
      isCrunchPngs = false
      isMinifyEnabled = false
      proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
      signingConfig = signingConfigs.getByName("release")
    }
    debug { }
  }
  compileOptions {
    sourceCompatibility = JavaVersion.VERSION_11
    targetCompatibility = JavaVersion.VERSION_11
  }
  buildFeatures {
    compose = true
    buildConfig = true
  }
  testOptions { unitTests { isIncludeAndroidResources = true } }
  dependenciesInfo {
    includeInApk = false
    includeInBundle = true
  }
}

dependencies {
  implementation(platform(libs.androidx.compose.bom))
  implementation(libs.androidx.activity.compose)
  implementation(libs.androidx.compose.material.icons.core)
  implementation(libs.androidx.compose.material.icons.extended)
  implementation(libs.androidx.compose.material3)
  implementation(libs.androidx.compose.ui)
  implementation(libs.androidx.compose.ui.graphics)
  implementation(libs.androidx.compose.ui.tooling.preview)
  implementation(libs.androidx.core.ktx)
  implementation(libs.androidx.lifecycle.runtime.compose)
  implementation(libs.androidx.lifecycle.runtime.ktx)
  implementation(libs.androidx.lifecycle.viewmodel.compose)
  implementation(libs.kotlinx.coroutines.android)
  implementation(libs.kotlinx.coroutines.core)
  testImplementation(libs.androidx.compose.ui.test.junit4)
  testImplementation(libs.androidx.core)
  testImplementation(libs.androidx.junit)
  testImplementation(libs.junit)
  testImplementation(libs.kotlinx.coroutines.test)
  testImplementation(libs.robolectric)
  testImplementation(libs.roborazzi)
  testImplementation(libs.roborazzi.compose)
  testImplementation(libs.roborazzi.junit.rule)
  androidTestImplementation(platform(libs.androidx.compose.bom))
  androidTestImplementation(libs.androidx.compose.ui.test.junit4)
  androidTestImplementation(libs.androidx.espresso.core)
  androidTestImplementation(libs.androidx.junit)
  androidTestImplementation(libs.androidx.runner)
  debugImplementation(libs.androidx.compose.ui.test.manifest)
  debugImplementation(libs.androidx.compose.ui.tooling)
}
