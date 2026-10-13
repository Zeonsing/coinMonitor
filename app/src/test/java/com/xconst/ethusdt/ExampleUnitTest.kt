package com.xconst.ethusdt

import okhttp3.Request
import org.json.JSONArray
import org.json.JSONObject
import org.junit.Test

import org.junit.Assert.*

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * See [testing documentation](http://d.android.com/tools/testing).
 */
class ExampleUnitTest {
    @Test
    fun fuck(): Unit {


            var a = arrayOf("BTC_USDT","ETH_USDT")
            a.forEach {

//            val url = "$baseUrl/api/prices?instIds=$instIds&key=$API_KEY&ts=$timestamp&sign=$sign"
                val url = "https://api.gateio.ws/api/v4/futures/usdt/tickers?contract=$it"
//        val url = "$baseUrl/api/prices?instIds=$instIds&key=$API_KEY&ts=$timestamp&sign=$sign"
//        Log.d("", "fetchPrices: "+url)
//        val startTime = System.currentTimeMillis()

                val request = Request.Builder()
                    .url(url)
                    .get()
                    .build()

                client.newCall(request).execute().use { response ->
//            val responseTime = System.currentTimeMillis()
//            println("请求响应延迟: ${responseTime - startTime}ms")


                    if (!response.isSuccessful) {
                        throw Exception("HTTP ${response.code}")
                    }



                    val body = response.body?.string() ?: return

                    val json = JSONObject(body)
                    val data: JSONArray = json.getJSONArray("data")

//                for (i in 0 until data.length()) {
//                    val item = data.getJSONObject(i)
//
//                    val instId = item.getString("instId")
//                    val price = item.getDouble("price")
//
//                    val symbol = Symbol.Companion.fromInstId(instId) ?: continue
//
//                    onPrice(symbol, price)
//                }
                }
            }


        }

}