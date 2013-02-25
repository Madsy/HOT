#ifndef BINGAPI_HPP_GUARD
#define BINGAPI_HPP_GUARD

#include <cmath>
#include <utility>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <curl/curl.h>
#include <QPixmap>

namespace Bing
{
  class TileSystem
  {
  private:

    static const double EarthRadius = 6378137.0;
    static const double MinLatitude = -85.05112878;
    static const double MaxLatitude = 85.05112878;
    static const double MinLongitude = -180.0;
    static const double MaxLongitude = 180.0;
 
    CURL* m_Handle;
    double m_X, m_Y, m_Zoom;

  public:

    static const int TileSize = 256;
    
    TileSystem() : m_X(0), m_Y(0), m_Zoom(0)
    {
      curl_global_init(CURL_GLOBAL_ALL);
      m_Handle = curl_easy_init();
      if(!m_Handle){
	std::runtime_error ex("Curl runtime error.");
	throw ex;
      }
    }

    ~TileSystem()
    {
      curl_easy_cleanup(m_Handle);
    }

    static double clip(double n, double minValue, double maxValue)
    {
      return std::min(std::max(n, minValue), maxValue);
    }

    double X() const
    {
        return m_X;
    }

    double Y() const
    {
        return m_Y;
    }

    double Zoom() const
    {
        return m_Zoom;
    }

    void setX(double x)
    {
        m_X = x;
    }

    void setY(double y)
    {
        m_Y = y;
    }

    void setZoom(double zoom)
    {
        m_Zoom = zoom;
    }

    static unsigned int mapSize(int levelOfDetail)
    {
      return (uint) TileSystem::TileSize << levelOfDetail;
    }

    static double groundResolution(double latitude, int levelOfDetail)
    {
      latitude = TileSystem::clip(latitude, MinLatitude, MaxLatitude);
      return std::cos(latitude * M_PI / 180.0) * 2.0 * M_PI * EarthRadius / TileSystem::mapSize(levelOfDetail);
    }

    static double mapScale(double latitude, double levelOfDetail, double screenDpi)
    {
      return TileSystem::groundResolution(latitude, levelOfDetail) * screenDpi / 0.0254;
    }

    static void latLongToPixelXY(double latitude, double longitude, double levelOfDetail, int& pixelX, int& pixelY)
    {
      latitude = TileSystem::clip(latitude, MinLatitude, MaxLatitude);
      longitude = TileSystem::clip(longitude, MinLongitude, MaxLongitude);

      double x = (longitude + 180.0) / 360.0; 
      double sinLatitude = std::sin(latitude * M_PI / 180.0);
      double y = 0.5 - std::log((1.0 + sinLatitude) / (1.0 - sinLatitude)) / (4.0 * M_PI);

      double mapSize = TileSystem::mapSize(levelOfDetail);
      pixelX = (int) TileSystem::clip(x * mapSize + 0.5, 0.0, mapSize - 1.0);
      pixelY = (int) TileSystem::clip(y * mapSize + 0.5, 0.0, mapSize - 1.0);
    }

    static void pixelXYToLatLong(int pixelX, int pixelY, int levelOfDetail, double& latitude, double& longitude)
    {
      double mapsize = TileSystem::mapSize(levelOfDetail);
      double x = (TileSystem::clip(pixelX, 0.0, mapsize - 1.0) / mapsize) - 0.5;
      double y = 0.5 - (TileSystem::clip(pixelY, 0.0, mapsize - 1.0) / mapsize);

      latitude = 90.0 - 360.0 * std::atan(std::exp(-y * 2.0 * M_PI)) / M_PI;
      longitude = 360.0 * x;
    }

    static void pixelXYToTileXY(int pixelX, int pixelY, int& tileX, int& tileY)
    {
      tileX = pixelX / 256;
      tileY = pixelY / 256;
    }

    static void tileXYToPixelXY(int tileX, int tileY, int& pixelX, int& pixelY)
    {
      pixelX = tileX * 256;
      pixelY = tileY * 256;
    }

    static std::string tileXYToQuadKey(int tileX, int tileY, int levelOfDetail)
    {
#if 0
        std::string quadKey = "0";

        for (int i = levelOfDetail; i > 0; i--){
            char value = 0;
            int mask = 1 << (i - 1);
            if ((tileX & mask) != 0){
                value++;
            }
            if ((tileY & mask) != 0){
                value++;
                value++;
            }
            quadKey += (char)('0' + value);
        }
        return quadKey;
#endif
        std::string quadkey("a");
        for(int i=levelOfDetail-1; i>=0; i--){
          int num = 0;
          int mask = 1 << i;
          if(tileX & mask) num |= 1;
          if(tileY & mask) num |= 2;
          quadkey += (char)('0' + num);
        }
        return quadkey;
    }

    static void quadKeyToTileXY(const std::string& quadKey, int& tileX, int& tileY, int& levelOfDetail)
    {
      tileX = tileY = 0;
      levelOfDetail = quadKey.length();
      for (int i = levelOfDetail; i > 0; i--){
	int mask = 1 << (i - 1);
	switch (quadKey[levelOfDetail - i]){
	case '0':
	  break;
	case '1':
	  tileX |= mask;
	  break;
	case '2':
	  tileY |= mask;
	  break;
	case '3':
	  tileX |= mask;
	  tileY |= mask;
	  break;
	default:
	  {
	    std::runtime_error ex("Invalid quadkey");
	    throw ex;
	  }
	}
      }
    }
    
    void downloadTile(const std::string& path, const std::string& url)
    {
        CURLcode err;
	curl_easy_setopt(m_Handle, CURLOPT_URL, url.c_str());
	FILE* f = fopen(path.c_str(), "w");
	if(!f){
	  std::runtime_error ex("Curl runtime error.");
	  throw ex;
	}
	curl_easy_setopt(m_Handle, CURLOPT_WRITEDATA, (void*)f);
	err = curl_easy_perform(m_Handle);
	curl_easy_reset(m_Handle);
	fclose(f);
	if(err){
	  std::runtime_error ex("Curl runtime error.");
	  throw ex;
	}
    }
    QPixmap* getTile(const std::string& downloadPath, int x, int y, int levelOfDetail)
    {
      QPixmap* f = NULL;
      std::string quad = Bing::TileSystem::tileXYToQuadKey(x, y, levelOfDetail);
      std::string filename = quad + ".jpeg";
      std::string placeholderFilename("placeholder.jpeg");
      std::string url = "http://ecn.t0.tiles.virtualearth.net/tiles/" + filename + "?g=0";
      
      f = new QPixmap(std::string(downloadPath + '/' + filename).c_str());
      if(f->isNull()){
          delete f;
        /* File doesn't exist, so download it. */
      downloadTile(downloadPath + '/' + filename, url);
        f = new QPixmap(std::string(downloadPath + '/' + filename).c_str());
        if(!f){
          f = new QPixmap(std::string(downloadPath + '/' + placeholderFilename).c_str());
        }
      }
      return f;
    }
  };
}


#endif

