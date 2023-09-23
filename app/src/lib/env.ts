export const apiHost = new URL(import.meta.env.PROD 
  ? window.location.href
  : (import.meta.env.VITE_API_HOST ?? "http://localhost"));

export const apiEndpoint = new URL("/api/", apiHost);

export const apiUrl = apiEndpoint.href;