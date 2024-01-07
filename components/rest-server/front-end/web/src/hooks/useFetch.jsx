
import { useEffect, useState } from 'react';

const useFetch = (url) => {
  const [data, setData] = useState(null);
  const [status, setStatus] = useState();
  const getData = async () => {
    setStatus('loading');
    console.log('loading')
    try {
      const config =
      {
        method: "GET",
        headers: {
          "Content-Type": "application/json",
        },
      }
      const resp = await fetch(url, config);
      const respData = await resp?.json();

      setData(respData);
      setStatus('loaded');
      console.log('loaded')
      return respData;
    }
    catch (error) {
      setStatus(error);
      console.log(error)
    }
  }

  useEffect(() => {
    getData()
  }, [])
  return [data, setData, getData, status];
};

export default useFetch;
