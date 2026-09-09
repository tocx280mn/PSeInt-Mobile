package com.example

import android.content.Context
import androidx.test.core.app.ApplicationProvider
import org.junit.Assert.assertEquals
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import org.robolectric.annotation.Config
import org.robolectric.annotation.GraphicsMode

@RunWith(RobolectricTestRunner::class)
@Config(sdk = [36])
@GraphicsMode(GraphicsMode.Mode.NATIVE)
class ExampleRobolectricTest {

  @Test
  fun `read string from context`() {
    val context = ApplicationProvider.getApplicationContext<Context>()
    val appName = context.getString(R.string.app_name)
    assertEquals("PSeInt Mobile", appName)
  }

  @Test
  fun `all institutional profiles load once with readable descriptions`() {
    val context = ApplicationProvider.getApplicationContext<Context>()
    val profiles = PSeIntProfile.loadAllProfiles(context)
    assertEquals(profiles.size, profiles.map { it.name.lowercase() }.distinct().size)
    org.junit.Assert.assertTrue(profiles.size > 100)
    org.junit.Assert.assertFalse(profiles.any { it.description.contains('\uFFFD') })
  }
}
